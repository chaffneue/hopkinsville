#ifndef LcdNavigator_h
  #define LcdNavigator_h
  #include <LiquidCrystal_I2C.h>
  
  class Navigation;
  
  class NavigationItem {
    friend class Navigation;

    public:
      NavigationItem(uint8_t lcdRow, uint8_t lcdColumn, uint8_t rangeMax, uint8_t value, char* clearChar, Navigation* navigation, void (*lcdPrinterCallback)(uint8_t));
      
      void setNextNavigationItem(NavigationItem* next);
      void setPreviousNavigationItem(NavigationItem* previous);
      void incrementValue();
      void decrementValue();
      
    private:
      Navigation *navigation;
      NavigationItem *previous, *next;
      uint8_t lcdRow, lcdColumn, value, rangeMax;
      char *clearChar;
      void (*lcdPrinterCallback)(uint8_t);
  };

  class Navigation {
    public: 
      Navigation();

      void addNavigationItem(NavigationItem& navigationItem);
      void addLcdInstance(LiquidCrystal_I2C& lcd);
      void editNext();
      void editPrevious();
      void refresh();
      void updateEditable();
      void clearEditable();
      void incrementEditableValue();
      void decrementEditableValue();
      void selectNextEditable();
      void selectPreviousEditable();
      
    private:
      NavigationItem *first, *last, *current, *editing;
      LiquidCrystal_I2C *lcd;
  };
#endif
