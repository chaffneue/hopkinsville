#include "LcdNavigator.h"
#include <stdlib.h>

/** Initialize a navigation item at a specific LCD coordinate 
 *  @param lcdRow - the lcd row to use eg 0 or 1 
 *  @param lcdColum - the lcd start column eg. 0-16
 *  @param rangeMax - the maximum value for the parameter
 *  @param clearChar - the clear character(s) to use when in edit mode
 *  @param navigation - the navigation context
 */
NavigationItem::NavigationItem(uint8_t lcdRow, uint8_t lcdColumn, uint8_t rangeMax, uint8_t value, char* clearChar, Navigation* navigation, void (*lcdPrinterCallback)(uint8_t)) {
  navigation->addNavigationItem(*this);
  
  this->lcdRow = lcdRow;
  this->lcdColumn = lcdColumn;
  this->rangeMax = rangeMax;
  this->value = value;
  this->clearChar = clearChar;
  this->navigation = navigation;
  this->lcdPrinterCallback = lcdPrinterCallback;
}

/** Set the previous link for the nav item
 *  @param previous - the previous navigation item reference or NULL
 */
void NavigationItem::setPreviousNavigationItem(NavigationItem* previous) {
  this->previous = previous;
}

/** Set the next line for the nav item
 *  @param next the next - navigation item reference or NULL
 */
void NavigationItem::setNextNavigationItem(NavigationItem* next) {
  this->next = next;
} 

/** Increment the value of the navigation item
 */
void NavigationItem::incrementValue() {
  if(this->value < this->rangeMax) {
    this->value++;
  }
}

/** Decrement the value of the navigation item
 */
void NavigationItem::decrementValue() {
  if(this->value > 0) {
    this->value--;
  }
}

/** Constructor 
 */
Navigation::Navigation(){}

/** Create a linked list of navigation items
 *  @param navigationItem a navigation item reference
 */
void Navigation::addNavigationItem(NavigationItem& navigationItem) {
  if(first == NULL) {
    first = &navigationItem;
    editing = first;
  } else {
    navigationItem.setPreviousNavigationItem(last);
    last->next = &navigationItem;
  }
  
  last = &navigationItem;
}

/**
 * Add the i2c LCD instance to the Navigation handler
 * @param lcd an lcd instance
 */
void Navigation::addLcdInstance(LiquidCrystal_I2C& lcd) { 
  this->lcd = &lcd;
}

/** Refresh all naviagtion items
 */
void Navigation::refresh() {
  current = first;
  while(current) {
    lcd->setCursor(current->lcdColumn, current->lcdRow);
    current->lcdPrinterCallback(current->value);
    current = current->next; 
  }
}

/** Update the editable area with input from the rotary knob
 */
void Navigation::updateEditable() {
  lcd->setCursor(editing->lcdColumn, editing->lcdRow);
  editing->lcdPrinterCallback(editing->value);
}

/** Blink the editable value to show it's being actively edited
 */
void Navigation::clearEditable() {
  lcd->setCursor(editing->lcdColumn, editing->lcdRow);
  lcd->print(editing->clearChar);
}

/** Increment the value for the current editable navitem 
 */
void Navigation::incrementEditableValue() {
  editing->incrementValue();
}

/** Decrement the value for the current editable nav item
 */
void Navigation::decrementEditableValue() {
  editing->decrementValue();
}

/** Select the next field to edit
 */
void Navigation::selectNextEditable() {
  if(editing->next != NULL) {
    editing = editing->next;
  } else {
    editing = first;  
  }
}

/** Select the previous field to edit
 */
void Navigation::selectPreviousEditable() {
  if(editing->previous != NULL) {
    editing = editing->previous;
  } else {
    editing = last;  
  }
}

