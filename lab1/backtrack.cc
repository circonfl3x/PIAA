#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <raylib.h>
#include <thread>
#include <time.h>
#include <vector>
#define BWIDTH 1000
#define BHEIGHT 800

// Global variables
std::vector<int> queens;  
int currentRow = 0;
int currentCol = 0;
bool isSolving = false;
int solutions = 0;
long long steps = 0;
int queensCount = 8;  
int cellSize = 60;     
int offX, offY;
std::string status = "Press SPACE to start";
Texture2D queenTexture;

Color BROWN_LIGHT = {222, 184, 135, 255};
Color BROWN_DARK = {139, 69, 19, 255};
Color BROWN_HIGHLIGHT = {255, 215, 0, 255};

void DrawBoard();
bool isSafe(int row, int col);
void solveNQueens(int row);
void reset();
void RunPerformanceTest();

bool isSafe(int row, int col) {
  for (int i = 0; i < queens.size(); i++) {
    int qCol = queens[i];  // Direct vector access (O(1))
    if (qCol == col || abs(i - row) == abs(qCol - col)) {
      return false;
    }
  }
  return true;
}

void solveNQueens(int row) {
  if (!isSolving)
    return; 

  if (row == queensCount) {
    solutions++;
    status = "Found solution #" + std::to_string(solutions);
    DrawBoard();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return;
  }

  for (int col = 0; col < queensCount; col++) {
    if (!isSolving)
      return;

    currentRow = row;
    currentCol = col;
    steps++;

    DrawBoard();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (isSafe(row, col)) {
      queens.push_back(col);  // Changed from add() to push_back

      status = "Place queen " + std::to_string(row + 1) + " at [" +
               std::to_string(row) + "," + std::to_string(col) + "]";
      DrawBoard();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      solveNQueens(row + 1);

      if (!isSolving)
        return;

      if (row + 1 < queensCount || queens.size() == queensCount) {
        status = "Backtrack from queen " + std::to_string(row + 1);
        queens.pop_back();  // Changed from pop() to pop_back
        DrawBoard();
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
      }
    }
  }
}

int countSolutions(int n) {
  std::vector<int> testQueens; 
  int count = 0;

  std::function<void(int)> solve = [&](int row) {
    if (row == n) {
      count++;
      return;
    }

    for (int col = 0; col < n; col++) {
      bool safe = true;
      for (int i = 0; i < testQueens.size(); i++) {
        int qCol = testQueens[i];  
        if (qCol == col || abs(i - row) == abs(qCol - col)) {
          safe = false;
          break;
        }
      }

      if (safe) {
        testQueens.push_back(col); 
        solve(row + 1);
        testQueens.pop_back();  
      }
    }
  };

  solve(0);
  return count;
}

void DrawBoard() {
  BeginDrawing();
  ClearBackground(RAYWHITE);

  int uiOffsetX = 50;  
  
  int maxBoardDimension = std::max(queensCount, 8);
  cellSize = std::min(60, (BWIDTH - 300) / maxBoardDimension); 
  cellSize = std::min(cellSize, (BHEIGHT - 200) / maxBoardDimension);
  
  offX = (GetScreenWidth() - queensCount * cellSize) / 2; 
  offY = 120; 

  DrawText("N-QUEENS (Recursive Backtracking)", uiOffsetX, 20, 30, DARKBROWN);
  
  if (isSolving) {
    DrawText("STATUS: SOLVING...", uiOffsetX, 60, 20, ORANGE);
  } else {
    DrawText("STATUS: READY", uiOffsetX, 60, 20, GREEN);
  }

  DrawText(TextFormat("Board Size: %d x %d", queensCount, queensCount), 
           GetScreenWidth() - 250, 20, 20, DARKBROWN);
  DrawText(TextFormat("Steps: %lld", steps), GetScreenWidth() - 250, 50, 20, DARKGRAY);
  DrawText(TextFormat("Solutions: %d", solutions), GetScreenWidth() - 250, 80, 20, DARKGRAY);
  
  DrawText(status.c_str(), uiOffsetX, 90, 16, DARKGRAY);

  DrawText("SPACE: Start/Pause | +/-: Change # of queens | R: Reset | ESC: Exit",
           uiOffsetX, GetScreenHeight() - 30, 16, DARKGRAY);

  // Draw chessboard
  for (int row = 0; row < queensCount; row++) {
    for (int col = 0; col < queensCount; col++) {
      int x = offX + col * cellSize;
      int y = offY + row * cellSize;

      Color cellColor;
      if ((row + col) % 2 == 0) {
        cellColor = BROWN_LIGHT;
      } else {
        cellColor = BROWN_DARK;
      }

      if (isSolving && row == currentRow && col == currentCol) {
        cellColor = BROWN_HIGHLIGHT;
      }

      DrawRectangle(x, y, cellSize - 1, cellSize - 1, cellColor);

      // Draw queen if present using texture
      for (int i = 0; i < queens.size(); i++) {  
        if (i == row && queens[i] == col) {  
          // Draw queen texture
          Rectangle destRect = {(float)x, (float)y, (float)cellSize - 1,
                                (float)cellSize - 1};
          DrawTexturePro(
              queenTexture,
              {0, 0, (float)queenTexture.width, (float)queenTexture.height},
              destRect, {0, 0}, 0.0f, WHITE);

          // Draw queen number (not needed anymore)
          // DrawText(TextFormat("%d", i + 1), x + cellSize - 20, y + 5, 16,
                   // BLACK);
          break;
        }
      }

      DrawRectangleLines(x, y, cellSize - 1, cellSize - 1, BLACK);
    }
  }

  // Draw row labels (these move with the board but are positioned to the left)
  for (int i = 0; i < queensCount; i++) {
    DrawText(TextFormat("Q%d", i + 1), offX - 40, offY + i * cellSize + 10, 16,
             i < queens.size() ? GREEN : GRAY);  
  }

  EndDrawing();
}

void reset() {
  queens.clear();  
  currentRow = 0;
  currentCol = 0;
  isSolving = false;
  steps = 0;
  solutions = 0;
  status = "Press SPACE to start";
}

void RunPerformanceTest() {
  std::cout << "ИССЛЕДОВАНИЕ ВРЕМЕНИ ВЫПОЛНЕНИЯ ОТ РАЗМЕРА КВАДРАТА\n";
  std::cout << "Рекурсивный бэктрекинг (N-Queens проблема)\n";
  std::cout << "========================================================\n\n";

  std::cout << std::left;
  std::cout << std::setw(12) << "Размер N\t"  << std::setw(16) << "Время (мкс)\t" 
            << std::setw(14) << "Решения\t" << std::setw(16) << "Коэф. роста\t" << std::endl;
            

  std::cout << std::string(58, '-') << std::endl;

  long long prevTime = 0;

  for (int n = 2; n <= 14; n++) {
    auto start = std::chrono::high_resolution_clock::now();
    int solCount = countSolutions(n);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double growth = (prevTime > 0) ? (double)duration.count() / prevTime : 1.0;

    std::cout << std::left;
    std::cout << std::setw(12) << n << std::setw(16) << duration.count()
              << std::setw(14) << solCount << std::setw(16) << std::fixed
              << std::setprecision(2) << growth << "x\n";

    prevTime = duration.count();
  }

  std::cout << std::string(58, '-') << std::endl;
}

int main(void) {
  RunPerformanceTest();

  std::cout << "\nPress Enter to continue to visualization...";
  std::cin.get();

  // Initialize window
  InitWindow(BWIDTH, BHEIGHT, "N-Queens Recursive Backtracking");
  SetTargetFPS(60);

  // Load queen texture
  Image queenImage = LoadImage("./WQ.png");
  if (queenImage.data == NULL) {
    std::cout << "Failed to load queen texture! Using fallback drawing."
              << std::endl;
  }
  queenTexture = LoadTextureFromImage(queenImage);
  UnloadImage(queenImage);

  reset();

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD)) {
      queensCount = std::min(12, queensCount + 1);  // Increased max to 12 to match performance test
      reset();
    }
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT)) {
      queensCount = std::max(1, queensCount - 1);
      reset();
    }

    if (IsKeyPressed(KEY_SPACE)) {
      if (!isSolving) {
        reset();
        isSolving = true;
        status = "Solving for " + std::to_string(queensCount) + " queens...";

        std::thread solver([]() {
          solveNQueens(0);
          isSolving = false;
          if (solutions > 0) {
            status = "Found " + std::to_string(solutions) + " solutions";
          } else {
            status = "No solutions found";
          }
        });
        solver.detach();
      } else {
        isSolving = false;
        status = "Paused";
      }
    }

    if (IsKeyPressed(KEY_R)) {
      reset();
    }

    DrawBoard();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Unload texture
  UnloadTexture(queenTexture);
  CloseWindow();
  return 0;
}
