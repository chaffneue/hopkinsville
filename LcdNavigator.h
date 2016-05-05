#ifndef LcdNavigator_h
  #define LcdNavigator_h
  #include <LiquidCrystal_I2C.h>
  
  class Navigation;
  
  class NavigationItem {
    friend class Navigation;

    public:
      NavigationItem(int lcdRow, int lcdColumn, int rangeMax, int value, char* clearChar, Navigation* navigation, void (*lcdPrinterCallback)(int));
      
      void setNextNavigationItem(NavigationItem* next);
      void setPreviousNavigationItem(NavigationItem* previous);
      void incrementValue();
      void decrementValue();
      
    private:
      Navigation *navigation;
      NavigationItem *previous, *next;
      int lcdRow, lcdColumn, value, rangeMax;
      char *clearChar;
      void (*lcdPrinterCallback)(int);
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
