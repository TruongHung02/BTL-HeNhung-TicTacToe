#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_MOVE 12
#define BUTTON_OK 14
#define OLED_RESET 4

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

int gameMode = 0;   // 0 = human vs cpu, 1 = human vs human
int gameStatus = 0; // 0 = ending, 1 = in game
int whosplaying = 0;

int winner = -1; // 0 = Playing, 1 = Human, 2 = CPU (human 2), 3- Draft

int board[] = {0, 0, 0,
               0, 0, 0,
               0, 0, 0}; // 0 = blank, 1 = human (circle), 2 = computer (cross)/

//--------------------------------------------------------------------------------------------------------
void playhuman(int value)
{

  int humanMove = 0;

  bool stayInLoop = true;          // Xử lý mỗi lần người chơi nhấn nút
  bool showDot = false;            // Dẩu ? có nên xuất hiện hay ko
  long timerPos = millis() - 1000; // Thời gian mỗi lần ? xuất hiện

  while (stayInLoop)
  {
    // Kiểm tra vị trí người chơi move dến có phải là vị trí ngoài bảng hay đã được đánh rồi hay ko.
    // Nếu phải thì lập tức chuyển human move đến vị trí mới thích hợp
    while (board[humanMove] != 0)
    {
      humanMove++;
      if (humanMove > 8)
        humanMove = 0;
    }

    // Thời gian mỗi lần dấu ? xuất hiện
    if (timerPos + 200 < millis())
    {
      timerPos = millis();
      showDot = !showDot;
      playhuman_showpos(humanMove, showDot);
    }

    // Nếu người chơi bấm vào button_move cập nhật humonMove và xóa hiển thị dấu ? ở vị trí cũ
    if (digitalRead(BUTTON_MOVE) == LOW)
    {
      playhuman_showpos(humanMove, false);
      humanMove++;
      if(humanMove > 8) humanMove = 0;

      // Phòng th người chơi nhấn giữ button quá lâu, khi thả nút nhần thì mới trò chơi mới tiếp tục xử lý
      while (digitalRead(BUTTON_MOVE) == LOW)
        ;

      bool showDot = false;
      long timerPos = -1000;
    }

    // Nếu người chơi quyết định đánh thì thoát vòng lặp
    if (digitalRead(BUTTON_OK) == LOW)
      stayInLoop = false;
  }

  // Cập nhật giá trị mà người chơi đánh vào bảng
  board[humanMove] = value;
}

//------------------------------------------------------------------
// Dùng để hiển thị dấu ?
void playhuman_showpos(int humanMove, bool showDot)
{
  display.setTextSize(2);
  if (humanMove == 0)
    display.setCursor(5, 5);
  else if (humanMove == 1)
    display.setCursor(25, 5);
  else if (humanMove == 2)
    display.setCursor(45, 5);
  else if (humanMove == 3)
    display.setCursor(5, 25);
  else if (humanMove == 4)
    display.setCursor(25, 25);
  else if (humanMove == 5)
    display.setCursor(45, 25);
  else if (humanMove == 6)
    display.setCursor(5, 45);
  else if (humanMove == 7)
    display.setCursor(25, 45);
  else if (humanMove == 8)
    display.setCursor(45, 45);

  if (showDot)
    display.setTextColor(WHITE);
  else
    display.setTextColor(BLACK);

  display.print("?");
  display.display();
}
//--------------------------------------------------------------------------------------------------------

void playcpu()
{

  // Kiểm tra nước đi có thể dành chiến thắng, cpu có giá trị là 2 trong board
  // Nếu cpumove >=0 thì tồn tại nước đi mà cpu có thể dành chiến thắng
  // cpumove = -1 thì ko tồn tại nước đi mà cpu có thể thắng
  int cpumove = checkboard(2);

  // Nếu tồn tại nước đi mà cpu có thể thắng thì đánh luôn
  if (cpumove >= 0)
  {
    board[cpumove] = 2;
  }
  else
  {
    // Kiểm tra nước đi mà human có thể thắng và chặn họ
    cpumove = checkboard(1);
    if (cpumove >= 0)
    {
      board[cpumove] = 2;
    }

    // Nếu không tồn tại nước đi mà cpu và human có thể thắng thì cpu chọn bừa 1 vị trí và random đánh
    while (cpumove < 0)
    {
      int randomMove = random(10);
      if (randomMove >= 0 && randomMove <= 8 && board[randomMove] == 0)
      {
        cpumove = randomMove;
      }
    }
    board[cpumove] = 2;
  }
}

//--------------------------------------------------------------------------------------------------------
// Kiểm tra vị trí cần đánh để người chơi x có thể dành chiến thắng
// x = 1 - human/human 1, x = 2 - cpu/human2
int checkboard(int x)
{

  if (board[0] == 0 && board[1] == x && board[2] == x)
    return 0;

  else if (board[0] == x && board[1] == 0 && board[2] == x)
    return 1;

  else if (board[0] == x && board[1] == x && board[2] == 0)
    return 2;
  //-------------------------------------------------
  else if (board[3] == 0 && board[4] == x && board[5] == x)
    return 3;

  else if (board[3] == x && board[4] == 0 && board[5] == x)
    return 4;

  else if (board[3] == x && board[4] == x && board[5] == 0)
    return 5;
  //-------------------------------------------------
  else if (board[6] == 0 && board[7] == x && board[8] == x)
    return 6;

  else if (board[6] == x && board[7] == 0 && board[8] == x)
    return 7;

  else if (board[6] == x && board[7] == x && board[8] == 0)
    return 8;

  //-------------------------------------------------
  else if (board[0] == 0 && board[3] == x && board[6] == x)
    return 0;

  else if (board[0] == x && board[3] == 0 && board[6] == x)
    return 3;

  else if (board[0] == x && board[3] == x && board[6] == 0)
    return 6;

  //-------------------------------------------------
  else if (board[1] == 0 && board[4] == x && board[7] == x)
    return 1;

  else if (board[1] == x && board[4] == 0 && board[7] == x)
    return 4;

  else if (board[1] == x && board[4] == x && board[7] == 0)
    return 7;
  //-------------------------------------------------
  else if (board[2] == 0 && board[5] == x && board[8] == x)
    return 2;

  else if (board[2] == x && board[5] == 0 && board[8] == x)
    return 5;

  else if (board[2] == x && board[5] == x && board[8] == 0)
    return 8;

  //-------------------------------------------------
  else if (board[0] == 0 && board[4] == x && board[8] == x)
    return 0;

  else if (board[0] == x && board[4] == 0 && board[8] == x)
    return 4;

  else if (board[0] == x && board[4] == x && board[8] == 0)
    return 8;
  //-------------------------------------------------
  else if (board[2] == 0 && board[4] == x && board[6] == x)
    return 2;
  else if (board[2] == x && board[4] == 0 && board[6] == x)
    return 4;

  else if (board[2] == x && board[4] == x && board[6] == 0)
    return 6;

  else
    return -1;
}

//--------------------------------------------------------------------------------------------
// Kiểm tra trên board có người chơi nào thắng chưa và cập nhật lên biến winner
void checkWinner()
{ // check the board to see if there is a winner

  winner = 3; // 3=draft, 1= winner->human/human2, 2=winner->cpu/human2

  // circles win?
  if (board[0] == 1 && board[1] == 1 && board[2] == 1)
    winner = 1;
  else if (board[3] == 1 && board[4] == 1 && board[5] == 1)
    winner = 1;
  else if (board[6] == 1 && board[7] == 1 && board[8] == 1)
    winner = 1;
  else if (board[0] == 1 && board[3] == 1 && board[6] == 1)
    winner = 1;
  else if (board[1] == 1 && board[4] == 1 && board[7] == 1)
    winner = 1;
  else if (board[2] == 1 && board[5] == 1 && board[8] == 1)
    winner = 1;
  else if (board[0] == 1 && board[4] == 1 && board[8] == 1)
    winner = 1;
  else if (board[2] == 1 && board[4] == 1 && board[6] == 1)
    winner = 1;

  // crosses win?
  else if (board[0] == 2 && board[1] == 2 && board[2] == 2)
    winner = 2;
  else if (board[3] == 2 && board[4] == 2 && board[5] == 2)
    winner = 2;
  else if (board[6] == 2 && board[7] == 2 && board[8] == 2)
    winner = 2;
  else if (board[0] == 2 && board[3] == 2 && board[6] == 2)
    winner = 2;
  else if (board[1] == 2 && board[4] == 2 && board[7] == 2)
    winner = 2;
  else if (board[2] == 2 && board[5] == 2 && board[8] == 2)
    winner = 2;
  else if (board[0] == 2 && board[4] == 2 && board[8] == 2)
    winner = 2;
  else if (board[2] == 2 && board[4] == 2 && board[6] == 2)
    winner = 2;

  if (winner == 3)
  {
    for (int i = 0; i < 9; i++)
      if (board[i] == 0)
        winner = 0;
  }
}

//--------------------------------------------------------------------------------------------------------------
// reset các tham số global mỗi khi trò chơi kết thúc
void resetGame()
{
  for (int i = 0; i < 9; i++)
    board[i] = 0;
  winner = 0;
  gameStatus = 0;
}

//--------------------------------------------------------------------------------------------------------------

void boardDrawing()
{

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.drawFastHLine(0, 21, 64, WHITE);
  display.drawFastHLine(0, 42, 64, WHITE);

  display.drawFastVLine(21, 0, 64, WHITE);
  display.drawFastVLine(42, 0, 64, WHITE);

  // drawing the content of the  nine cells: " ", "o", "x"
  display.setTextSize(2);
  display.setCursor(5, 5);
  display.print(charBoard(0));
  display.setCursor(25, 5);
  display.print(charBoard(1));
  display.setCursor(45, 5);
  display.print(charBoard(2));
  display.setCursor(5, 25);
  display.print(charBoard(3));
  display.setCursor(25, 25);
  display.print(charBoard(4));
  display.setCursor(45, 25);
  display.print(charBoard(5));
  display.setCursor(5, 45);
  display.print(charBoard(6));
  display.setCursor(25, 45);
  display.print(charBoard(7));
  display.setCursor(45, 45);
  display.print(charBoard(8));
  display.display();

  delay(200);
}

//--------------------------------------------------------------------------------------------------------------
String charBoard(int x)
{
  if (board[x] == 0)
    return " ";
  if (board[x] == 1)
    return "o";
  if (board[x] == 2)
    return "x";

  return "?";
}

// Giao diện chọn chế độ
void menu()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 10);
  display.print("Select Mode:");
  display.setCursor(10, 30);
  display.print("1. P vs CPU");
  display.setCursor(10, 40);
  display.print("2. P vs P");
  display.display();
}

// Xử lý giao diện chọn chế độ
void showGameModeMenu()
{
  menu();
  gameMode = 0;

  while (true)
  {
    if (digitalRead(BUTTON_MOVE) == LOW)
    {
      while (digitalRead(BUTTON_MOVE) == LOW)
        ;

      gameMode = (gameMode + 1) % 2;

      if (gameMode == 0)
      {
        menu();
        display.setCursor(80, 30);
        display.print(">");
        display.setCursor(80, 40);
        display.print(" ");
      }
      else
      {
        menu();
        display.setCursor(80, 30);
        display.print(" ");
        display.setCursor(80, 40);
        display.print(">");
      }
      display.display();
    }

    if (digitalRead(BUTTON_OK) == LOW)
    {
      while (digitalRead(BUTTON_OK) == LOW)
        ;
      break;
    }
  }

  // Clear the display after exiting the menu loop
  display.clearDisplay();
  display.display();
}
//--------------------------------------------------------------------------------------------------------------
void setup()
{

  randomSeed(analogRead(0));

  // Cấu hình chân GIPO ở mức logic HIGH theo mặc đinh
  // Người dùng không bấm thì mức logic của 2 button này là 1 => Người dùng nhấn thì mức logic giảm về 0
  pinMode(BUTTON_MOVE, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();

  whosplaying = 2;
  while (whosplaying < 0 || whosplaying > 1)
    whosplaying = random(2);
}

//--------------------------------------------------------------------------------------------------------------
void loop()
{
  // Kiểm tra trạng thái trò chơi kết thúc hay chưa
  if (gameStatus == 0)
  {
    showGameModeMenu();
    resetGame();
    boardDrawing();
    gameStatus = 1;
    winner = 0;
  }

  //---------------------------------------------
  // Người vs Máy
  if (gameMode == 0)
  {
    // Trò chơi vẫn còn tiếp tục
    if (gameStatus == 1)
    {
      // Lặp cho đến khi tìm được winner (winner=3 là hòa)
      while (winner == 0)
      {
        display.setTextSize(2);

        if (whosplaying == 0)
        {
          display.setCursor(72, 25);
          display.print("CPU");
          display.display();
          delay(1000);

          playcpu();

          whosplaying = 1;
        }
        else
        {
          display.setCursor(72, 25);
          display.print("You");
          display.display();

          playhuman(1);

          whosplaying = 0;
        }

        boardDrawing();
        delay(500);

        checkWinner();

        if (winner > 0)
        {

          if (winner == 3)
          {
            display.setTextSize(2);
            display.setCursor(68, 25);
            display.print("Draft");
          }
          else
          {
            // showing who's the winner
            display.setTextSize(2);
            display.setCursor(72, 25);
            if (winner == 1)
            {
              Serial.println(F("You"));
              display.print("You");
              display.setCursor(72, 45);
              display.print("win");
            }
            else
            {
              Serial.println(F("CPU"));
              display.print("CPU");
              display.setCursor(72, 45);
              display.print("wins");
            }
          }
          display.display();
          delay(1000);
          
          while (digitalRead(BUTTON_MOVE) == HIGH && digitalRead(BUTTON_OK) == HIGH);
        }
      }

      gameStatus = 0;
      delay(1000);
    }
  }
  else if (gameMode == 1)
  {
    if (gameStatus == 1)
    {

      while (winner == 0)
      {

        display.setTextSize(2);

        if (whosplaying == 0)
        {

          display.setCursor(72, 25);
          display.print("P1");
          display.display();
          delay(1000);

          playhuman(1);

          whosplaying = 1; 
        }
        else
        {

          display.setCursor(72, 25);
          display.print("P2");
          display.display();

          playhuman(2);

          whosplaying = 0;
        }

        boardDrawing();
        delay(500);

        checkWinner();

        if (winner > 0)
        {

          if (winner == 3)
          {
            display.setTextSize(2);
            display.setCursor(68, 25);
            display.print("Draft");
          }
          else
          {
            // showing who's the winner
            display.setTextSize(2);
            display.setCursor(72, 25);
            if (winner == 1)
            {
              Serial.println(F("P1"));
              display.print("P1");
              display.setCursor(72, 45);
              display.print("win");
            }
            else
            {
              Serial.println(F("P2"));
              display.print("P2");
              display.setCursor(72, 45);
              display.print("wins");
            }
          }
          display.display();
          delay(1000);

          while (digitalRead(BUTTON_MOVE) == HIGH && digitalRead(BUTTON_OK) == HIGH);
        }
      }

      gameStatus = 0;
      delay(1000);
    }
  }
}
