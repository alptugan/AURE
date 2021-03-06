//
//  info.h
//  superShapes
//
//  Created by alp tugan on 16/01/17.
//
//

#ifndef info_h
#define info_h

class info {
    public:
    bool enable;
    bool help;
    ofTrueTypeFont title;
    ofTrueTypeFont helpText;
    ofxLabel text;
    
    info() {
        enable = true;
    };
    
    ~info() {
        
    };
    
    void setup() {
        //title.load("../../../../../../assets/fonts/DIN.otf", 25);
        //helpText.load("../../../../../../assets/fonts/DIN.otf", 10);
        title.setLineHeight(30);
        text.setName("The superformula is a generalization of the superellipse and was proposed \nby Johan Gielis around 2000. Gielis suggested that the formula can be\nused to describe many complex shapes and curves that are found in nature.\nGielis holds a patent related to the synthesis of patterns generated by the\nsuperformula.");
        text.setPosition(20, 95);
        
        
        
    }
    
    void draw() {
        if(enable) {
            ofPushStyle();
            ofSetColor(255);
            title.drawString("SUPERFORMULA\nALGORITHMS", 20, 45);
            //text.draw();
            ofPopStyle();
        }
        
        if(help) {
            ofPushStyle();
            ofSetColor(0,150);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
            
            ofSetColor(255);
            
            
            string txt = "- Press f for fullscreen\n-Press g to show/hide User Interface\n-Press ESC to exit application\n\nDeveloped by Alp Tugan\ninfo@alptugan.com\nwww.alptugan.com\nwww.filiktatasarim.com";
            
            ofRectangle s =  helpText.getStringBoundingBox(txt, 0, 0);
            helpText.drawString(txt, ofGetWidth()*0.5 - s.getWidth()*0.5, ofGetHeight()*0.5 - s.getHeight()*0.5);
            ofPopStyle();
        }
    }
    
    
    void setInfoVisible(bool _val) {
        enable = _val;
    }
    
    void setHelpVisible(bool _val) {
        help = _val;
    }
    
};
#endif /* info_h */
