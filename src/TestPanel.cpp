#include <stdlib.h>
#include <fstream>

#include "fltk/events.h"
#include "fltk/SharedImage.h"


#include "TestPanel.hpp"
#include "Logger.hpp"
#include "SoundManager.hpp"
#include "settings.hpp"
#include "Util.hpp"

TestPanel * TestPanel::TheTestPanel = 0;
#define WEIGHTED_PENALTY -3

TestPanel::TestPanel(int _x, int _y, int _width, int _height) : Group(_x, _y, _width, _height)
{
    currentItem = new Widget(0, 0, 0, 0);
    add(currentItem);

    counterItem = new Widget(0, 0, 20, 20);
    add(counterItem);
    counter = 0;

    currentState = NOT_STARTED;
    currentPosition = 0;
    incorrectClicks = 0;
    incorrectIgnores = 0;
    
    SoundManager::init();

    darkeningItem = new Widget(0, 0, _width, _height);
    add(darkeningItem);
    darkeningItem->hide();

    char * darkImageFile = "dark.png";
    ifstream file(darkImageFile, ifstream::in | ifstream::binary);
    SharedImage *image = 0;
    if (!file.fail())
    {
        file.close();
        image = SharedImage::get(darkImageFile);
        if (image)
        {
            darkeningItem->box(image);
        }
    }
    else
    {
        LOG_ERROR("Cannot read darkening file: " << darkImageFile);
    }
    int i = find(currentItem);
    LOG_DEBUG("Index of currentItem=" << i);
    int j = find(darkeningItem);
    LOG_DEBUG("Index of darkeningItem=" << j);

    take_focus();
}

void TestPanel::start(Group * parent, User selectedUser, TestMode mode, DataSet selectedDataSet, Scenario selectedScenario)
{
    LOG_DEBUG("TestPanel::start()...");
    fltk::register_images();
    
    LOG_DEBUG("TestPanel::start: dataset=" << selectedDataSet.toString());
    if (!TheTestPanel)
    {
        if (!parent)
        {
            LOG_ERROR("internal error: TheTestPanel::display() avec parent=0");
            exit(1);
        }
        int width = parent->w();
        int height = parent->h();
        TheTestPanel = new TestPanel(0, 0, width, height);

        parent->add(TheTestPanel);
    }

    //showIntro()
    TheTestPanel->scenario = selectedScenario;
    TheTestPanel->dataset = selectedDataSet;
    TheTestPanel->mode = mode;
    TheTestPanel->user = selectedUser;
    
    TheTestPanel->listOfItems = TheTestPanel->scenario.generateItemList(selectedDataSet);
    
    vector<string>::iterator i;
    string dump = "listOfItems=[";
    for (i = TheTestPanel->listOfItems.begin(); i != TheTestPanel->listOfItems.end(); i++)
    {
        if (i != TheTestPanel->listOfItems.begin()) dump += ", ";
        dump += *i;
    }
    dump += "]";
    LOG_DEBUG(dump);
    
    TheTestPanel->startTest();
    TheTestPanel->show();
}

void TestPanel::showIntro()
{
    
}

void TestPanel::hideIntro()
{
}

void TestPanel::startTest()
{
    hideIntro();
    currentState = RUNNING;
    LOG_INFO("Starting test...");
    
    // create the currentItem widget
    currentPosition = -1;
    next();
}

void TestPanel::processClick()
{
}

int TestPanel::handle(int eventType)
{
    int eventConsumed = 0;
    switch(eventType)
    {
        case FOCUS:
            // accept the focus attempt
            // in order to be able to process subsequent keyboard events
            LOG_DEBUG("TestPanel: take the focus");
            return 1;
            break;
        case ENTER:
            // take the focus by returning non-zero
            // necessary for handling keyboard events
            LOG_DEBUG("ENTER");
            eventConsumed = 1;
            break;
        case TIMEOUT:
            LOG_DEBUG("received TIMEOUT");
            if (currentState == DING_PENDING)
            {
                // the DING timer elapsed. go to next item
                currentState = RUNNING;
                next();
            }
            else if (currentState == RUNNING)
            {
                // the user DID NOT click on the item
                bool result = evaluateUserAction(false);
            
                storeResult(result, false);
                    
                currentState = RUNNING;
                next();
            }
            eventConsumed = 1;
            break;

        case KEY:
        {
            int key = fltk::event_key();
            if (' ' == key)
            {
                currentState = CLICK_PENDING;
                // and continue below to case PUSH:
                // (no 'break')
            }
            else
            {
                // ignore this event
                break;
            }
        }
        case PUSH:
            // the user DID click on the item
            LOG_DEBUG("PUSH");
            if ( (currentState == RUNNING) || (currentState == CLICK_PENDING) )
            {
                remove_timeout(); // cancel previous timeout (if any)
            
                bool result = evaluateUserAction(true);
                storeResult(result, true);
                if (!result)
                {
                    playDing();
                    //add_timeout(0.5);
                    //currentState = DING_PENDING;
                }

                // darken the picture
                darkenItem(); 
                eventConsumed = 1;

                currentState = CLICK_PENDING;
            }
            break;

        case KEYUP:
            if (currentState != CLICK_PENDING)
            {
                // ignore this event
                break;
            }

            // else: proceed with the RELEASE case
            
        case RELEASE:
            LOG_DEBUG("RELEASE");
            if (currentState != CLICK_PENDING)
            {
                // ignore this event
                break;
            }

            // cancel darkening
            darkeningItem->hide();

            next();
            eventConsumed = 1;

            break;

        default:
            LOG_DEBUG("event=" << eventType);
            break;
    }

    return eventConsumed;
}

void TestPanel::playDing()
{
    string dingItem = dataset.path + '/' + "ding.wav";
    SoundManager::playSound(dingItem, DING_CHANNEL);
}

bool TestPanel::evaluateUserAction(bool clicked)
{
    // check if the user clicked on the previous or not
    bool correct = true;
    
    // TODO invert the result if mode == ATTENTION
    
    if (currentPosition > 0)
    {
        string currentItemId = listOfItems[currentPosition];
        
        // check result of the previous item
        LOG_DEBUG("evaluateUserAction: currentItemId=" << currentItemId);
        if (clicked)
        {
            if (dataset.specifiedPictures.find(currentItemId) != dataset.specifiedPictures.end())
            {
                // the user made a mistake
                incorrectClicks++;
                correct = false;
                LOG_INFO("incorrect click on: " << currentItemId);
            }
        }
        else
        {
            if (dataset.specifiedPictures.find(currentItemId) == dataset.specifiedPictures.end())
            {
                // the user made a mistake
                incorrectIgnores++;
                correct = false;
                LOG_INFO("incorrect ignore on: " << currentItemId);
            }
        }
    }
    LOG_DEBUG("evaluateUserAction: correct=" << correct);
    return correct;

}

string TestPanel::getSoundFile(string itemId)
{
    // if itemId is like 'mammoth.png'
    // the expected sound file is : 'mammoth.wav'
    
    LOG_DEBUG("getSoundFile: " << itemId);
    string expectedSoundFile;
    int position = itemId.find_last_of('.');
    if (position == string::npos)
    {
        expectedSoundFile = itemId;
    }
    else
    {
        expectedSoundFile = itemId.substr(0, position);
    }
    expectedSoundFile += ".wav"; // extension of a sound file in format WAV
    return dataset.path + '/' + expectedSoundFile;
}
void TestPanel::next()
{
    static char counterString[50];
    counter++;

    LOG_DEBUG("TestPanel::next()...");
    t0 = Util::getTimestamp();
    
    remove_timeout(); // cancel previous timeout (if any)

    // display the next item (if any)
    if ( (currentPosition == -1) || 
         (currentPosition < listOfItems.size()-1) )
    {
        currentPosition++;
        string itemId = listOfItems[currentPosition];
        
        displayItem(itemId);

        sprintf(counterString, "%d", counter);
        counterItem->label(counterString);
        
        // look if a sound is related to the current item
        string soundFile = getSoundFile(itemId);
        if (soundFile.size() > 0)
        {
            SoundManager::playSound(soundFile, ITEM_CHANNEL);
        }
    
        add_timeout(8); // TODO period
    }
    else // finished
    {
        endScreen();
    }
    redraw();
}

void TestPanel::displayItem(string itemId)
{
    LOG_DEBUG("TestPanel::displayItem()... ");

    string itemPath = dataset.path + '/' + itemId;
    
    LOG_DEBUG("itemId=" << itemId);
    currentItem->hide();

    // try opening the file
    ifstream file(itemPath.c_str(), ifstream::in | ifstream::binary);
    SharedImage *image = 0;
    if (!file.fail())
    {
        file.close();
        image = SharedImage::get(itemPath.c_str());
        if (image)
        {

            // get the size of the image
            int width;
            int height;
            image->measure(width, height);
            LOG_DEBUG("image width=" << width << ", image height=" << height);

            scaleImage(width, height, w(), h());
            
            // put the image on the test panel
            currentItem->box(image);
            
            // resize and center in the test panel
            int x0 = (w() - width)/2;
            int y0 = (h() - height)/2;
            currentItem->resize(x0, y0, width, height);
            LOG_DEBUG("displayItem: new size[" << itemId << "]=" << width << "x" << height);
            currentItem->label(0);
        }
    }
    
    if (!image)
    {
        // the file could not be opened
        currentItem->label(itemId.c_str());
        currentItem->resize(10, 10, w()-20, h()-20); // same size as the panel
        currentItem->box(BORDER_FRAME);
        currentItem->labelsize(FONT_SIZE);
    }
    currentItem->show();
}
void TestPanel::darkenItem()
{
    LOG_DEBUG("TestPanel::darkenItem()... ");

    // put the image on the test panel
    darkeningItem->show();
    redraw();

    int i = find(currentItem);
    LOG_DEBUG("Index of currentItem=" << i);
    int j = find(darkeningItem);
    LOG_DEBUG("Index of darkeningItem=" << j);

}

void TestPanel::scaleImage(int &imageWidth, int &imageHeight, int windowWidth, int windowHeight)
{
    int newWidth = 0;
    int newHeight = 0;
    // first, try to make the width fit in the window
    newWidth = windowWidth;
    double scaleRatio = double(newWidth)/imageWidth;
    newHeight = imageHeight * scaleRatio;
    
    LOG_DEBUG("windowWidth=" << windowWidth << ", windowHeight=" << windowHeight);
    LOG_DEBUG("scaleImage(): 1. scaleRatio=" << scaleRatio << ", newWidth=" << newWidth <<
            ", newHeight=" << newHeight << ", windowHeight=" << windowHeight);
    if (newHeight > windowHeight)
    {
        // the x scaling was not enough.
        // do the y scaling
        newHeight = windowHeight;
        scaleRatio = double(newHeight)/imageHeight;
        newWidth = imageWidth * scaleRatio;
        LOG_DEBUG("scaleImage(): 2. scaleRatio=" << scaleRatio << ", newHeight=" <<
                newHeight << ", newWidth=" << newWidth);

    }
    imageWidth = newWidth;
    imageHeight = newHeight;
}


void TestPanel::endScreen()
{
    LOG_DEBUG("TestPanel::endScreen()...");
    SoundManager::close();

    remove_timeout(); // cancel previous timeout (if any)
    currentState = ENDED;
    
    // TODO evaluate latest click
    currentItem->hide();
    
    Widget * message = new Widget(100, 100, 200, 200, "End");
    message->box(FLAT_BOX);
    add(message);
    
    // collect the result and store it into the user file
    vector<pair<int, double> >::iterator i;
    ostringstream res;
    res << "result=[";
    for (i = resultTable.begin(); i != resultTable.end(); i++)
    {
        if (i != resultTable.begin()) res << ", ";
        res << "(" << i->first << ", " << i->second << ")";
    }
    res << "]";
    
    LOG_DEBUG(res.str());
    
    int finalScore = evaluateFinalScore();
    double clickSpeed = evaluateClickSpeed();
    user.storeNewResult(scenario, dataset, mode, finalScore, clickSpeed);
    
}


void TestPanel::storeResult(bool correctness, bool hasClicked)
{
    pair<int, double> intermediateResult;
    
    if (correctness)
    {
        intermediateResult.first = 1;
    }
    else
    {
        // error
        if (hasClicked && (mode == INHIBITION) )
        {
            intermediateResult.first = WEIGHTED_PENALTY;
        }
        else if ( (!hasClicked) && (mode == ATTENTION) )
        {
            intermediateResult.first = WEIGHTED_PENALTY;
        }
        else
        {
            intermediateResult.first = -1;
        }
    }
    
    intermediateResult.second = Util::getElapsedTime(t0);
    
    resultTable.push_back(intermediateResult);
    
}

double TestPanel::evaluateClickSpeed()
{
    vector<pair<int, double> >::iterator i;
    double averageClickSpeed = 0;
    for(i = resultTable.begin(); i != resultTable.end(); i++)
    {
        averageClickSpeed += i->second;
    }
    // normalize (to have a 100 scale
    averageClickSpeed /= scenario.numberOfItems;
    LOG_DEBUG("average click speed=" << averageClickSpeed);
    
    return averageClickSpeed;
}

int TestPanel::evaluateFinalScore()
{
    vector<pair<int, double> >::iterator i;
    int finalScore = scenario.numberOfItems;
    for(i = resultTable.begin(); i != resultTable.end(); i++)
    {
        finalScore += i->first;
    }
    // normalize (to have a 100 scale
    finalScore *= 100 / scenario.numberOfItems;
    LOG_DEBUG("Final score=" << finalScore);
    
    return finalScore;
}


