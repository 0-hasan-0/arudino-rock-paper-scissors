#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Mega 2560 PRO: SDA = Pin 20, SCL = Pin 21
LiquidCrystal_I2C lcd(0x27, 20, 4); 

// --- PIN DEFINITIONS ---
const int btnRock     = 2; 
const int btnPaper    = 3; 
const int btnScissors = 4; 
const int btnMenu     = 5; 
const int btnPlus     = 6; // Pin 6: Increases Value (+100 Stakes / +1000 Goal)
const int btnMinus    = 7; // Pin 7: Decreases Value (-100 Stakes / -1000 Goal)

// --- GAME VARIABLES ---
long score = 1000;         
long goal = 20000;
int stakes = 1000; 
int winCount = 0;          
int lossCount = 0;         

// --- LOGIC TRACKING ---
unsigned long buttonPressTime = 0;
bool isMenuOpen = false;
bool lastMenuState = HIGH;
bool resetTriggered = false;

// --- SCROLLING TEXT ---
String creditText = "    original idea by sleepknight8    ";
int scrollPos = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 500; // Slightly faster scroll

void setup() {
  pinMode(btnRock, INPUT_PULLUP);
  pinMode(btnPaper, INPUT_PULLUP);
  pinMode(btnScissors, INPUT_PULLUP);
  pinMode(btnMenu, INPUT_PULLUP);
  pinMode(btnPlus, INPUT_PULLUP);
  pinMode(btnMinus, INPUT_PULLUP);
  
  lcd.begin();
  lcd.backlight();
  randomSeed(analogRead(0));
  
  refreshGameDisplay();
}

void loop() {
  handleMenuButton();
  
  if (isMenuOpen) {
    handleMenuAdjustments();
    updateScrollingCredit(); 
  } else {
    handleInGameAdjustments();
    // Game Inputs
    if (digitalRead(btnRock) == LOW)     { play(0); delay(300); }
    if (digitalRead(btnPaper) == LOW)    { play(1); delay(300); }
    if (digitalRead(btnScissors) == LOW) { play(2); delay(300); }
  }
}

// Adjusts the STAKES (Points won/lost)
void handleInGameAdjustments() {
  if (digitalRead(btnPlus) == LOW) {
    stakes += 100;
    refreshGameDisplay();
    delay(150);
  }
  if (digitalRead(btnMinus) == LOW) {
    if (stakes > 100) stakes -= 100;
    refreshGameDisplay();
    delay(150);
  }
}

// Adjusts the GOAL (Points to reach)
void handleMenuAdjustments() {
  if (digitalRead(btnPlus) == LOW) {
    goal += 1000;
    drawMenuBase();
    delay(150);
  }
  if (digitalRead(btnMinus) == LOW) {
    if (goal > 1000) goal -= 1000;
    drawMenuBase();
    delay(150);
  }
}

void handleMenuButton() {
  bool currentState = digitalRead(btnMenu);
  if (currentState == LOW && lastMenuState == HIGH) {
    buttonPressTime = millis();
    resetTriggered = false;
    lastMenuState = LOW;
  }
  
  if (currentState == LOW && !resetTriggered) {
    if (millis() - buttonPressTime > 3000) {
      score = 1000;
      winCount = 0;
      lossCount = 0;
      resetTriggered = true;
      lcd.clear();
      lcd.setCursor(0, 1); lcd.print("   SYSTEM RESET   ");
      delay(1500);
      isMenuOpen = false;
      refreshGameDisplay();
    }
  }

  if (currentState == HIGH && lastMenuState == LOW) {
    if (!resetTriggered) {
      isMenuOpen = !isMenuOpen;
      if (isMenuOpen) drawMenuBase();
      else refreshGameDisplay();
    }
    lastMenuState = HIGH;
  }
}

void updateScrollingCredit() {
  if (millis() - lastScrollTime > scrollDelay) {
    lastScrollTime = millis();
    String displayStr = creditText.substring(scrollPos) + creditText.substring(0, scrollPos);
    lcd.setCursor(0, 3);
    lcd.print(displayStr.substring(0, 20)); 
    scrollPos++;
    if (scrollPos >= creditText.length()) scrollPos = 0;
  }
}

void drawMenuBase() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("== RPS STATS ==");
  lcd.setCursor(0, 1); lcd.print("Wins:" + String(winCount) + " Loses:" + String(lossCount));
  lcd.setCursor(0, 2); lcd.print("SET GOAL: " + String(goal));
  // Row 3 scroll handled by updateScrollingCredit
}

void play(int playerMove) {
  int computerMove = random(0, 3);
  String moves[] = {"ROCK", "PAPER", "SCISSORS"};
  
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("YOU: " + moves[playerMove]);
  lcd.setCursor(0,1); lcd.print("CPU: " + moves[computerMove]);

  if (playerMove == computerMove) {
    lcd.setCursor(0,2); lcd.print("RESULT: DRAW");
  } 
  else if ((playerMove == 0 && computerMove == 2) || 
           (playerMove == 1 && computerMove == 0) || 
           (playerMove == 2 && computerMove == 1)) {
    score += stakes;
    winCount++;
    lcd.setCursor(0,2); lcd.print("RESULT: WIN! +" + String(stakes));
  } 
  else {
    score -= stakes;
    lossCount++;
    lcd.setCursor(0,2); lcd.print("RESULT: LOSE! -" + String(stakes));
  }

  lcd.setCursor(0, 3); lcd.print("SCORE: "); lcd.print(score);
  delay(1500);

  if (score < 0) {
    lcd.clear();
    lcd.setCursor(0, 1); lcd.print("     GAME OVER!   ");
    lcd.setCursor(0, 2); lcd.print("    SCORE RESET   ");
    score = 1000; 
    delay(2500);
  }

  if (score >= goal) {
    lcd.clear();
    lcd.setCursor(0,1); lcd.print("  YOU DID IT!   ");
    lcd.setCursor(0,2); lcd.print("GOAL " + String(goal) + " REACHED!");
    while(1); 
  }
  
  refreshGameDisplay();
}

void refreshGameDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("--- RPS GAME ---");
  lcd.setCursor(0, 1); lcd.print("GOAL: " + String(goal));
  lcd.setCursor(0, 2); lcd.print("STAKES: " + String(stakes));
  lcd.setCursor(0, 3); lcd.print("SCORE: "); lcd.print(score);
}