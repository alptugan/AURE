//
//  globalParameters.h
//  rawAV
//
//  Created by alp tugan on 10.02.2020.
//

#ifndef globalParameters_h
#define globalParameters_h

static string settingsAudioFile;            // Audio settings path constant
static int w,h;                             // Current window width/height

const ofPrimitiveMode modes [] = {OF_PRIMITIVE_TRIANGLES,OF_PRIMITIVE_TRIANGLE_STRIP,OF_PRIMITIVE_TRIANGLE_FAN,OF_PRIMITIVE_LINES,OF_PRIMITIVE_LINE_STRIP,OF_PRIMITIVE_LINE_LOOP,OF_PRIMITIVE_POINTS};

static ofTrueTypeFontSettings settings3("fonts/AdihausDIN-Bold.ttf", 1000);
static ofTrueTypeFont fontAdidas_1000;

static ofTrueTypeFontSettings settings4("fonts/AdihausDIN-Bold.ttf", 28);
static ofTrueTypeFont rainerBold_28;

static ofTrueTypeFontSettings settings5("fonts/AdihausDIN-Bold.ttf", 200);
static ofTrueTypeFont rainer_200;

static ofTrueTypeFontSettings settings6("fonts/AdihausDIN-Bold.ttf", 350);
static ofTrueTypeFont fontAdidas350;

static ofTrueTypeFontSettings settings7("fonts/AdihausDIN-Bold.ttf", 120);
static ofTrueTypeFont adidas_120;

static ofTrueTypeFontSettings settings8("fonts/AdihausDIN-Bold.ttf", 28);
static ofTrueTypeFont rainerBold_15;


static string pathFBXModels = "../../../../../../assets/model/fbx";
static int posGuiConent1 = 425;
/*static int widthGui1    = 300;*/
#endif /* globalParameters_h */
