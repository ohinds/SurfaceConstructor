
#ifndef COLORS_H
#define COLORS_H

int NUMCOLORS = 12;
int NUMTACKCOLORS = 5;
int NUMLABELCOLORS = 1;
int FIRST_LABEL_COLOR = 27;

/* colors */
/* these colors can be tack colors */
const int RED = 0;
const int BLUE = 1;
const int GREEN = 2;
const int PURPLE = 3;
const int YELLOW = 4;

/* these colors cannot be tack colors */
const int BROWN = 5;
const int DARK_GRAY = 6;
const int WHITE = 7;
const int GRAY = 8;
const int LIGHT_GRAY = 9;
const int LIGHT_BLUE = 10;
const int ORANGE = 11;

/* grayed end stuff */
const int GRAYED_LIGHT_ORANGE = 24;
const int GRAYED_DARK_ORANGE = 25;
const int BLACK = 26;

const int CYAN = 27;

/* color vectors for ease of use*/
const float RED_V[] = {1.0, 0.0, 0.0};
const float BLUE_V[] = {0.0, 0.0, 1.0};
const float GREEN_V[] = {0.0, 1.0, 0.0};
const float PURPLE_V[] = {0.75, 0.0, 0.75};
const float DARK_GRAY_V[] = {0.2, 0.2, 0.2};
const float YELLOW_V[] = {1.0, 1.0, 0.0};
const float BROWN_V[] = {0.31, 0.13, 0.05};
const float WHITE_V[] = {1.0, 1.0, 1.0};
const float GRAY_V[] = {0.5, 0.5, 0.5};
const float LIGHT_GRAY_V[] = {0.7, 0.7, 0.7};
const float LIGHT_BLUE_V[] = {0.4, 0.4, 1.0};
const float ORANGE_V[] = {1.0, 0.5, 0.0};
const float BLACK_V[] = {0.0, 0.0, 0.0};
const float CYAN_V[] = {0.0, 1.0, 1.0};

/* grayed color vectors */
const float GRAYED_RED_V[] = {0.8, 0.4, 0.4};
const float GRAYED_BLUE_V[] = {0.4, 0.4, 0.8};
const float GRAYED_GREEN_V[] = {0.4, 0.8, 0.4};
const float GRAYED_PURPLE_V[] = {0.55, 0.0, 0.55};
const float GRAYED_YELLOW_V[] = {0.8, 0.8, 0.4};
const float GRAYED_BROWN_V[] = {0.5, 0.3, 0.25};
const float GRAYED_DARK_GRAY_V[] = {0.4, 0.4, 0.4};
const float GRAYED_WHITE_V[] = {0.8, 0.8, 0.8};
const float GRAYED_GRAY_V[] = {0.5, 0.5, 0.5};
const float GRAYED_LIGHT_GRAY_V[] = {0.5, 0.5, 0.5};
const float GRAYED_LIGHT_BLUE_V[] = {0.5, 0.5, 0.9};
const float GRAYED_ORANGE_V[] = {0.8, 0.5, 0.4};
const float GRAYED_LIGHT_ORANGE_V[] = {0.9, 0.6, 0.5};
const float GRAYED_DARK_ORANGE_V[] = {0.7, 0.4, 0.3};

/* array of the color vectors, to be indexed by the color names */
const float *COLORS[] = {
  RED_V, 
  BLUE_V, 
  GREEN_V, 
  PURPLE_V, 
  YELLOW_V, 
  BROWN_V, 
  DARK_GRAY_V, 
  WHITE_V, 
  GRAY_V, 
  LIGHT_GRAY_V, 
  LIGHT_BLUE_V, 
  ORANGE_V,
  GRAYED_RED_V, 
  GRAYED_BLUE_V, 
  GRAYED_GREEN_V, 
  GRAYED_PURPLE_V, 
  GRAYED_YELLOW_V, 
  GRAYED_BROWN_V, 
  GRAYED_DARK_GRAY_V, 
  GRAYED_WHITE_V, 
  GRAYED_GRAY_V, 
  GRAYED_LIGHT_GRAY_V, 
  GRAYED_LIGHT_BLUE_V, 
  GRAYED_ORANGE_V,
  GRAYED_LIGHT_ORANGE_V,
  GRAYED_DARK_ORANGE_V,
  BLACK_V,
  CYAN_V
};

#endif
