#ifdef _WX

#include "wx/wx.h"
#include "wx/event.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "typeint.h"
#include "undo.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "frame_tv.h"

#include "frame_f.h"
#include "presentationapi_f.h"
#include "structcreation_f.h"
#include "boxlocate_f.h"
#include "geom_f.h"
#include "font_f.h"
#include "content_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /* _GL */

#include "logdebug.h"


#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaCreateShapeEvtHandler.h"

static int m_mouse_x,m_mouse_y; 

static void DrawShape (int x1, int y1, int x2, int y2, int shape, int frameId)
{ 
  int lx,ly,tmp;
  int x3, y3, x4, y4, x5, y5, x6, y6;
  
  
  glEnable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_XOR);
  glColor4ub (127, 127, 127, 0);

  if(!(shape == 0 || (shape >= 12 && shape <= 14)))
    {
      /* It's a shape drawn in a rectangle */

      lx = abs(x2 - x1);
      ly = abs(y2 - y1);

      if(shape == 20)
	/* equilateral triangle */
	lx = (int) (floor(2 *  ly / sqrt(3)));
      else if(shape == 3 || shape == 15 || shape == 16 || shape == 23)
	{
	  /* lx and ly must be equal (square, circle...) */
	  if(ly < lx)lx=ly; else ly = lx;
	}

      if(x2 < x1){x2 = x1 - lx; tmp = x2; x2 = x1; x1 = tmp;}
	else x2 = x1 + lx;

      if(y2 < y1){y2 = y1 - ly; tmp = y2; y2 = y1; y1 = tmp;}
        else y2 = y1 + ly;
    }

  switch(shape)
    {
      /* Line */
    case 0:
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 
      break;

    case 12: /* Simple Arrow */

      x3 = x1; y3 = y1;
      x4 = x2; y4 = y2;
      GetArrowCoord(&x3, &y3, &x4, &y4);

      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 
	      
      glBegin(GL_LINE_STRIP);
      glVertex2i(x3, y3);
      glVertex2i(x2, y2);
      glVertex2i(x4, y4);

      glEnd ();
      break;



    case 13: /* Double Arrow */
      x3 = x1; y3 = y1;
      x4 = x2; y4 = y2;
      GetArrowCoord(&x3, &y3, &x4, &y4);

      x5 = x2; y5 = y2;
      x6 = x1; y6 = y1;
      GetArrowCoord(&x5, &y5, &x6, &y6);

      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y2);
      glEnd (); 


      glBegin(GL_LINE_STRIP);
      glVertex2i(x3, y3);
      glVertex2i(x2, y2);
      glVertex2i(x4, y4);
      glEnd (); 

      glBegin(GL_LINE_STRIP);
      glVertex2i(x5, y5);
      glVertex2i(x1, y1);
      glVertex2i(x6, y6);
      glEnd (); 

      break;

    case 14: /* Zigzag */
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1,y1);
      glVertex2i(x1,y1+(y2-y1)/2);
      glVertex2i(x2,y1+(y2-y1)/2);
      glVertex2i(x2,y2);
      glEnd (); 
      break;



      /* Selection */
    case 42:

      /* Foreign object */
    case 9:

      /* Square */
    case 15:

      /* Rectangle */
    case 1:
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y1);
      glVertex2i(x1 + lx, y1);
      glVertex2i(x1 + lx, y1 + ly);
      glVertex2i(x1, y1 + ly);
      glEnd (); 
      break;

      /* Rounded Square */
    case 16:
      /* Rounded-Rectangle */
    case 2:
      if(lx > 5 && ly > 5)
	{
	  glBegin(GL_LINE_LOOP);
	  glVertex2i(x1+5, y1);
	  glVertex2i(x1+lx-5, y1);
	  glVertex2i(x1+lx, y1+5);
	  glVertex2i(x1+lx, y1+ly-5);
	  glVertex2i(x1+lx-5, y1+ly);
	  glVertex2i(x1+5, y1+ly);
	  glVertex2i(x1, y1+ly-5);
	  glVertex2i(x1, y1+5);
	  glEnd();
	}
      break;

      /* Circle */
    case 3:
      /* Ellipse */
    case 4:
      GL_DrawArc (x1, y1, lx, ly, 0, 360, 0, FALSE);
      break;

    case 17: /* diamond */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y1 + ly/2);
      glVertex2i(x1 + lx/2, y1);
      glVertex2i(x2, y1 + ly/2);
      glVertex2i(x1 + lx/2, y2);
      glEnd ();
      break;

    case 18: /* trapezium */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y2);
      glVertex2i(x1 + lx / 4, y1);
      glVertex2i(x2 - lx / 4, y1);
      glVertex2i(x2, y2);
      glEnd (); 
      break;

    case 19: /* parallelogram */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1 + lx/4, y1);
      glVertex2i(x2, y1);
      glVertex2i(x2 - lx / 4, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 20: /* equilateral triangle */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1 + lx/2, y1);
      glVertex2i(x1 + lx, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 21: /* isosceles triangle */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1 + lx/2, y1);
      glVertex2i(x2, y2);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 22: /* rectangle triangle */
      glBegin(GL_LINE_LOOP);
      glVertex2i(x1, y1);
      glVertex2i(x2, y1);
      glVertex2i(x1, y2);
      glEnd (); 
      break;

    case 23: /* cube */
    case 24: /* parallelepiped */
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1+ly/4);
      glVertex2i(x1+lx/4, y1);
      glVertex2i(x2, y1);
      glVertex2i(x2, y2-ly/4);
      glVertex2i(x2-lx/4, y2);
      glVertex2i(x1,y2);
      glVertex2i(x1, y1+ly/4);
      glVertex2i(x2-lx/4, y1+ly/4);
      glVertex2i(x2-lx/4, y2);
      glEnd (); 

      glBegin(GL_LINE_STRIP);
      glVertex2i(x2-lx/4, y1+ly/4);
      glVertex2i(x2, y1);
      glEnd (); 
      break;

    case 25: /* Cylinder */
      glBegin(GL_LINE_STRIP);
      glVertex2i(x1, y1+ly/6);
      glVertex2i(x1, y2-ly/6);
      glEnd (); 

      glBegin(GL_LINE_STRIP);
      glVertex2i(x2, y1+ly/6);
      glVertex2i(x2, y2-ly/6);
      glEnd (); 

      GL_DrawArc (x1, y1, lx, ly/3, 0, 360, 0, FALSE);
      GL_DrawArc (x1, y2-ly/3, lx, ly/3, 0, -180, 0, FALSE);

      break;

    default:
      /* m_ShapeNumber = 10 = Text */

      break;

    }

  glLogicOp(GL_COPY);
  glDisable(GL_COLOR_LOGIC_OP);

#ifdef _WINDOWS
  GL_Swap (frameId);
#endif /* WINDOWS */
}


IMPLEMENT_DYNAMIC_CLASS(AmayaCreateShapeEvtHandler, wxEvtHandler)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaCreateShapeEvtHandler, wxEvtHandler)
EVT_CHAR( AmayaCreateShapeEvtHandler::OnChar )

EVT_LEFT_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown) // Process a wxEVT_LEFT_DOWN event. 
EVT_LEFT_UP(		AmayaCreateShapeEvtHandler::OnMouseUp) // Process a wxEVT_LEFT_UP event. 
EVT_LEFT_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick) // Process a wxEVT_LEFT_DCLICK event. 
EVT_MIDDLE_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown) // Process a wxEVT_MIDDLE_DOWN event. 
EVT_MIDDLE_UP(	AmayaCreateShapeEvtHandler::OnMouseUp) // Process a wxEVT_MIDDLE_UP event. 
EVT_MIDDLE_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick) // Process a wxEVT_MIDDLE_DCLICK event. 
EVT_RIGHT_DOWN(	AmayaCreateShapeEvtHandler::OnMouseDown) // Process a wxEVT_RIGHT_DOWN event. 
EVT_RIGHT_UP(		AmayaCreateShapeEvtHandler::OnMouseUp) // Process a wxEVT_RIGHT_UP event. 
EVT_RIGHT_DCLICK(	AmayaCreateShapeEvtHandler::OnMouseDbClick) // Process a wxEVT_RIGHT_DCLICK event. 
EVT_MOTION(		AmayaCreateShapeEvtHandler::OnMouseMove) // Process a wxEVT_MOTION event. 
EVT_MOUSEWHEEL(	AmayaCreateShapeEvtHandler::OnMouseWheel) // Process a wxEVT_MOUSEWHEEL event. 
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::AmayaCreateShapeEvtHandler() : wxEvtHandler()
{
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::AmayaCreateShapeEvtHandler(AmayaFrame * p_frame, int *x1, int *y1, int *x2, int *y2, int *nb_points, Document doc, int shape_number)
  : wxEvtHandler()
  ,m_IsFinish(false)
  ,m_pFrame(p_frame)
  ,m_FrameId(p_frame->GetFrameId())
  ,m_xmin(*x1)
  ,m_ymin(*y1)
  ,m_xmax(*x2)
  ,m_ymax(*y2)
  ,m_x1(x1)
  ,m_y1(y1)
  ,m_x2(x2)
  ,m_y2(y2)
  ,m_ShapeNumber(shape_number)
  ,m_NbPoints(nb_points)
  ,m_document(doc)
    
{

  *m_NbPoints = 0;
  if (m_pFrame)
    {
      /* attach this handler to the canvas */
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PushEventHandler(this);

      /* assign a cross mouse cursor */
      m_pFrame->GetCanvas()->SetCursor( wxCursor(wxCURSOR_CROSS) );
      m_pFrame->GetCanvas()->CaptureMouse();

    }

  if(m_ShapeNumber == 42)
    /* It is actually a box that allows to select graphical elements */
    InitDrawing (2, 1, 0);
  else
    InitDrawing (5, 1, 0);
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
AmayaCreateShapeEvtHandler::~AmayaCreateShapeEvtHandler()
{
	/* Clear the Shape */
    DrawShape (*m_x1, *m_y1, *m_x2, *m_y2, m_ShapeNumber, m_FrameId);
	
	if (m_pFrame)
    {
      /* detach this handler from the canvas (restore default behaviour) */
      AmayaCanvas * p_canvas = m_pFrame->GetCanvas();
      p_canvas->PopEventHandler(false /* do not delete myself */);
      
      /* restore the default cursor */
      m_pFrame->GetCanvas()->SetCursor( wxNullCursor );
      m_pFrame->GetCanvas()->ReleaseMouse();
    }

  
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
bool AmayaCreateShapeEvtHandler::IsFinish()
{
  return m_IsFinish;
}

/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnChar( wxKeyEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseDown
 * Description:  handle mouse button down events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseDown( wxMouseEvent& event )
{
  if (IsFinish() || *m_NbPoints != 0)return;

  /* Are we in the SVG ? */
  if(!MouseCoordinatesToSVG(m_document, m_pFrame, m_xmin, m_xmax, m_ymin, m_ymax,
			    FALSE, &m_mouse_x, &m_mouse_y))return;

  *m_x1 = m_mouse_x;
  *m_y1 = m_mouse_y;
  *m_NbPoints = 1;

  if(m_ShapeNumber == 10)
    {
      /* Only one point is needed */
      *m_x2 = *m_x1;
      *m_y2 = *m_y1;
      m_IsFinish = true;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseUp
 * Description:  handle mouse button up events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseUp( wxMouseEvent& event )
{
#define MIN_SHAPE_SIZE 10
  if(IsFinish() || *m_NbPoints != 1)return;

  /* Check that the shape is not too small */
  if(abs(m_mouse_x - *m_x1) < MIN_SHAPE_SIZE ||
     abs(m_mouse_y - *m_y1) < MIN_SHAPE_SIZE)return;

  /* Are we in the SVG ? */
  if(!MouseCoordinatesToSVG(m_document, m_pFrame, m_xmin, m_xmax, m_ymin, m_ymax,
      FALSE, &m_mouse_x, &m_mouse_y))return;

  *m_x2 = m_mouse_x;
  *m_y2 = m_mouse_y;
  *m_NbPoints = 2;
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseDbClick
 * Description:  handle mouse dbclick events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseDbClick( wxMouseEvent& event )
{
  m_IsFinish = true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseMove
 * Description:  handle mouse move events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseMove( wxMouseEvent& event )
{
  if(*m_NbPoints > 0)
    DrawShape (*m_x1, *m_y1, m_mouse_x, m_mouse_y, m_ShapeNumber, m_FrameId);

  m_mouse_x = event.GetX();
  m_mouse_y = event.GetY();

  MouseCoordinatesToSVG(m_document, m_pFrame, m_xmin, m_xmax, m_ymin, m_ymax,
			FALSE, &m_mouse_x, &m_mouse_y);

  if(*m_NbPoints > 0)
    DrawShape (*m_x1, *m_y1, m_mouse_x, m_mouse_y, m_ShapeNumber, m_FrameId);

#ifndef _WINDOWS
    m_pFrame->GetCanvas()->Refresh();
#endif
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCreateShapeEvtHandler
 *      Method:  OnMouseWheel
 * Description:  handle mouse wheel events
 -----------------------------------------------------------------------*/
void AmayaCreateShapeEvtHandler::OnMouseWheel( wxMouseEvent& event )
{
}


#endif /* _WX */