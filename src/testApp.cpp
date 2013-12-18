#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);

//####### GRT setup #######

    //Initialize the training and info variables
    infoText = "";
    gr.setup();
//####### End GRT setup #######

    traceLength = 20;
    //sender = new ofxOscSender;
    //sender.setup("192.168.0.14", 57120);
    sender.setup("192.168.0.14", 12001);


    one.pos = ofVec2f(0,0);

    camWidth 		= 320;	// try to grab at this size.
    camHeight 		= 240;

    colorImg.allocate(camWidth,camHeight);      //Image that will house the cameras output, used because of laziness

    colorImgHSV.allocate(camWidth,camHeight);   //our HSB image that will house the color image and deal out the Hue, Saturation and brightness

    hueImg.allocate(camWidth,camHeight);        //Hue map
    satImg.allocate(camWidth,camHeight);        //saturation map
    briImg.allocate(camWidth,camHeight);        //brightness map, not gonna be used but necessary

    reds.allocate(camWidth, camHeight);         //our postRange image basically

    colorTrackedPixelsRed = new unsigned char [camWidth * camHeight];     //rangeImage

    trackedTextureRed.allocate(camWidth, camHeight, GL_LUMINANCE);        //final output

    vidGrabber.listDevices();
    vidGrabber.setDeviceID(1);

    //vidGrabber.setVerbose(true);                    //just some text for debugging
    vidGrabber.initGrabber(camWidth,camHeight);     //start the show!
    //vidGrabber.loadMovie("diabolo.mp4");
    //vidGrabber.play();
}

//--------------------------------------------------------------
void testApp::update(){
    vidGrabber.update();                                                 //get a frame from the camera

    colorImg.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);    //remember that colorImg? put the camera image into it

    colorImgHSV = colorImg;                                                 //now we stuff the colorImg into our HSB image
    colorImgHSV.convertRgbToHsv();                                          //now we convert the colorImg inside colorImgHSV into HSV

    colorImgHSV.convertToGrayscalePlanarImages(hueImg, satImg, briImg);     //distribute the hue, saturation and brightness to hueImg, satImg, and briImg

    // As stated in the following discussion, due to a probable bug in ofxOpenCv addon,
    // you have to explicitly call flagImageChanged() after convertToGrayscalePlanarImages().
    // http://www.openframeworks.cc/forum/viewtopic.php?f=8&t=2693
    hueImg.flagImageChanged();
    satImg.flagImageChanged();
    briImg.flagImageChanged();

    // ok cool, here we go:
    unsigned char * huePixels = hueImg.getPixels();                         //huePixels is now a raw array of pixels
    unsigned char * satPixels = satImg.getPixels();                         //satPixels is now a raw array of pixels  just like huePixels
    //unsigned char * briPixels = briImg.getPixels();
    int nPixels = camWidth * camHeight;                                     //get the number of pixels in the images since these raw images are continuous, so no breaks
                                                                            //so pixel number camWidth + 1 would be the first pixels in the second row of pixels of the image

/* huePixels is a gigantic black and white array. so every pixel has a value from 0 to 255. This represents the hue values from the original color image. Certain colors can
be represented by certain hue ranges. hues from 4 to 21 are redish while 109 to 115 are green*/

    for (int i = 0; i < nPixels; i++){                                           //let's go through every pixel in hue pixels
        if ((huePixels[i] >= one.hue - 12 && huePixels[i] <= one.hue + 12) &&    //if the hue is of a certain range
            (satPixels[i] >= one.sat - 24 && satPixels[i] <= one.sat + 200)){    //if the saturation is of a certain range
            colorTrackedPixelsRed[i] = 255;                                      //mark this corresponding pixel white
        } else {
            colorTrackedPixelsRed[i] = 0;                                        //if it doesn't fit then color the corresponding pixel black
        }
    }

    reds.setFromPixels(colorTrackedPixelsRed, camWidth, camHeight);              //set reds from the colorTrackedPixelsRed array so it's all clean and openCv operable
    finderRed.findContours(reds, 10,nPixels/3, 1, false, true);                  //lets find one (1) blob in the grayscale openCv image reds

    trackedTextureRed.loadData(colorTrackedPixelsRed, camWidth, camHeight, GL_LUMINANCE);  //load up the data from the colorTrackedPixelsRed into a texture

//------------------------------

    if(finderRed.blobs.size() > 0) {
        one.pos = ofVec2f(finderRed.blobs[0].centroid.x,finderRed.blobs[0].centroid.y);  //if the blob exists, set it's associated color (one) to it's position
        gr.update(finderRed.blobs[0].centroid.x, finderRed.blobs[0].centroid.y);
    }



}

//--------------------------------------------------------------
void testApp::draw(){

    ofBackground(100,100,100);             //make a NYC style gray background

    ofSetHexColor(0xffffff);                  //set a white color as the setColor
    vidGrabber.draw(0,0);                  //draw our video for reference/viewing pleasure
	//colorImgHSV.draw(340, 0);

    //trackedTextureRed.draw(20, 300);       //draw everything that was found
    trackedTextureRed.draw(320, 0);
    //ofDrawBitmapString("red",20, 280);     //label
    finderRed.draw();                      //draw our contour tracker over the video

    //glPushMatrix();                        //start a new openGL stack
	//glTranslatef(20,300,0);            //translate lower a bit
	//finderRed.draw();                  //draw the contour tracker over the trackedTextureRed
    //glPopMatrix();                         //end the stack

    if(finderRed.blobs.size() > 0) {       //if the blob exists then state it's x and y

        if(jitter == true){
            xj = 0.9*xj + 0.1*finderRed.blobs[0].centroid.x;
            yj = 0.9*yj + 0.1*finderRed.blobs[0].centroid.y;            
        }
        else{
            xj = finderRed.blobs[0].centroid.x;
            yj = finderRed.blobs[0].centroid.y;
        }

        if(traceX.size() > traceLength){
            traceX.pop_back();
            traceY.pop_back();
        }

        traceX.insert(traceX.begin(), xj);
        traceY.insert(traceY.begin(), yj);

        char tempStr1[255];
        //sprintf(tempStr1, "x : %f\ny : %f", finderRed.blobs[0].centroid.x, finderRed.blobs[0].centroid.y);
        sprintf(tempStr1, "xj : %f\nyj : %f", xj, yj);

        message.setAddress("/hello");
        message.addFloatArg(xj);
        message.addFloatArg(yj);
        sender.sendMessage(message);
        message.clear();
        ofDrawBitmapString(tempStr1, 20, 250); //draw the string
    }

    ofSetColor(0);
    ofFill();
    ofRect(camWidth,0,camWidth,camHeight);

    ofSetColor(255);
    ofNoFill();

    ofBeginShape();
    for (unsigned i = 0; i < traceX.size(); ++i){
        ofCurveVertex(traceX.at(i)+camWidth, traceY.at(i));
    }
    ofEndShape();

    gr.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

    switch( key ){
        cout << key << endl;
        case 'j':
            toggleJitter();
            break;
        case 'r':
            gr.recordGesture();
            break;
        case 53:
            gr.decreaseClassLabel();
            break;
        case 45:
            gr.increaseClassLabel();
            break;
        case 't':
            gr.trainPipeline();
            break;
        case 's':
            gr.saveTrainingData();
            break;
        case 'c':
            gr.clearTrainingData();
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    unsigned char * huePixels = hueImg.getPixels();  //teh hue
    unsigned char * satPixels = satImg.getPixels();  //teh saturation
    /*unsigned char * briPixels = briImg.getPixels();  //teh brightness*/ //unnecessary really, hue and sat should be enough

    x = MIN(x,hueImg.width-1);     //find the smallest value out of those two so we don't crash if we click outside of the camera image
    y = MIN(y,hueImg.height-1);

    // Init for jitter
    xj = x;
    yj = y;

    if(button == 0) {
        one.hue = huePixels[x+(y*hueImg.width)];  //set the hue
        one.sat = satPixels[x+(y*satImg.width)];  //set the sat
        /*one.bri = briPixels[x+(y*briImg.width)];*/
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

void testApp::setJitter(bool s){
    jitter = s;
}

void testApp::toggleJitter(){
    if(jitter == true){
        setJitter(false);
        infoText = "Jitter disabled";
    }
    else{
        setJitter(true);
        infoText = "Jitter enabled";
    }
}

void testApp::setTrainingDataFilename(char* filename){
    gr.setTrainingDataFilename(filename);
}