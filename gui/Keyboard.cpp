#include "Keyboard.hpp"
#include "AppList.hpp"
#include "Feedback.hpp"

Keyboard::Keyboard(AppList* appList, std::string* myText, Feedback* feedback)
{
    this->x = 372;
    this->y = 417;

    if (appList && appList->R == 4)
        this->x = 240;

    this->appList = appList;
    this->feedback = feedback;
    this->myText = myText;

    // position the keyboard based on this x and y
    updateSize();
}

void Keyboard::render(Element* parent)
{
    if (hidden)
        return;

  SDL_Rect dimens = { this->x, this->y, this->width, this->height };

  this->window = parent->window;
  this->renderer = parent->renderer;

	SDL_SetRenderDrawColor(parent->renderer, 0xf9, 0xf9, 0xf9, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);

  for (int y=0; y<3; y++)
    for (int x=0; x<10 - y - (y==2); x++)
    {
      SDL_Rect dimens2 = {this->x + kXPad + x*kXOff + y*yYOff, this->y + kYPad + y*ySpacing, keyWidth, keyWidth};
      SDL_SetRenderDrawColor(parent->renderer, 0xf4, 0xf4, 0xf4, 0xff);
      SDL_RenderFillRect(parent->renderer, &dimens2);

        if (curRow ==y && index == x)
        {
            // draw the currently selected tile if these index things are set
            // TODO: check touchmode and decide whether to draw a filled rect or not
            SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
            SDL_RenderFillRect(parent->renderer, &dimens2);
        }
    }

//   SDL_Rect dimens3 = {this->x+dPos, this->y + dHeight, dWidth, textSize};
//   SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
//   SDL_RenderFillRect(parent->renderer, &dimens3);
//
//   SDL_Rect dimens4 = {this->x+sPos, this->y + dHeight, sWidth, textSize};
//   SDL_SetRenderDrawColor(parent->renderer, 0xff, 0xaa, 0xaa, 0xff);
//   SDL_RenderFillRect(parent->renderer, &dimens4);

  super::render(this);

}

bool Keyboard::process(InputEvents* event)
{
    if (hidden)
        return false;

    bool ret = false;
    
    // default to non touchmode, unless one of our inputables has something to say about it
    bool touchMode = ((appList == NULL)? false : appList->touchMode) || (feedback == NULL)? false : feedback->touchMode;
    
    if (touchMode)
        curRow = index = -1;
    
    if (touchMode)
    {
        if (curRow < 0) curRow = 0;
        if (index < 0) index = 0;
    }

  if (event->isTouchDown() && event->touchIn(this->x, this->y, width, height))
  {
      for (int y=0; y<3; y++)
          for (int x=0; x<10 - y - (y==2); x++)
              if (event->touchIn(this->x+kXPad + x*kXOff + y*yYOff, this->y + kYPad + y*ySpacing, keyWidth, keyWidth))
              {
                  ret |= true;
                  curRow = y;
                  index = x;
              }
      return true;
  }

if (event->isTouchUp())
{
    // only proceed if we've been touchdown'd
    // reset current row and info
    curRow = -1;
    index = -1;

    if (event->touchIn(this->x, this->y, width, height))
      {

        for (int y=0; y<3; y++)
          for (int x=0; x<10 - y - (y==2); x++)
            if (event->touchIn(this->x+kXPad + x*kXOff + y*yYOff, this->y + kYPad + y*ySpacing, keyWidth, keyWidth))
            {
                ret |= true;
                myText->push_back(std::tolower(rows[y][x*2]));
            }

        if (event->touchIn(this->x+dPos, this->y+dHeight, dWidth, textSize))
            if (!myText->empty())
            {
                ret |= true;
                myText->pop_back();
            }

        if (event->touchIn(this->x+sPos, this->y+dHeight, sWidth, textSize))
        {
            ret |= true;
            myText->append(" ");
        }

          if (ret && appList)
          {
        // update search results
        this->appList->y = 0;
        this->appList->update();
          }
          else if (ret && feedback)
          {
              // TODO: do this a more generic way (TypeableElement?) instead of passing in as more params
              this->feedback->refresh();
          }

          return ret;
      }

      return false;
    }

    return false;
}

void Keyboard::updateSize()
{
    this->elements.clear();

    this->width = 900;
    this->height = (304/900.0)*width;

    // set up lots of scaling variables based on the width/height

    this->keyWidth = (int)(0.08*width);
    this->padding = keyWidth/2.0;

    // these field variables are for displaying the QWERTY keys (touching and displaying)
    kXPad = (int)((23/400.0)*width);
    kXOff = (int)((36.5/400.0)*width);
    yYOff = (int)((22/400.0)*width);
    kYPad = (int)((17/135.0)*height);
    ySpacing = (int)((33/400.0)*width);

    // these local variables position only the text, and has nothing to do with the
    // touch. They should likely be based on the above field variables so those
    // can change quickly
    int kXPad = (int)((30/400.0)*width);
    int kXOff = (int)((22/400.0)*width);
    int kYPad = (int)((14/400.0)*width);
    int kYOff = (int)((33/400.0)*width);

    this->textSize = 0.9375*keyWidth;

    // delete and space key dimensions
    dPos = (int)((13/400.0)*width);
    dHeight = (int)((85/135.0)*height);
    sPos = (int)((326/400.0)*width);

    dWidth = (int)(1.4125*textSize);
    sWidth = (int)(1.91875*textSize);

    // set up the three rows into one vector
    this->rows = std::vector<std::string>();
    rows.push_back(row1);
    rows.push_back(row2);
    rows.push_back(row3);
    SDL_Color gray = { 0x52, 0x52, 0x52, 0xff };

    // go through and draw each of the three rows at the right position
    for (int x=0; x<rows.size(); x++)
    {
        TextElement* rowText = new TextElement(rows[x].c_str(), textSize, &gray, true);
        rowText->position(kXPad+x*kXOff, kYPad+x*kYOff);
        this->elements.push_back(rowText);
    }

    // these are local variables, similar to how the other ones are global events
    int dPos2 = (int)((20/400.0)*width);
    int dHeight2 = (int)((90/135.0)*height);
    int sPos2 = (int)((330/400.0)*width);

    int textSize2 = (int)((16/400.0)*width);

    TextElement* delButton = new TextElement("DEL", textSize2, &gray, false);
    delButton->position(dPos2, dHeight2);
    this->elements.push_back(delButton);

    TextElement* spaceButton = new TextElement("SPACE", textSize2, &gray, false);
    spaceButton->position(sPos2, dHeight2);
    this->elements.push_back(spaceButton);
}

Keyboard::~Keyboard()
{
    // during deconstructor,
    if (this->appList != NULL)
        this->appList->keyboard = NULL;
}
