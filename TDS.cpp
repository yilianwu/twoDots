#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/GLAux.h>
//==============================================
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines

//===================================================================================================================

using namespace std;
struct window_info //視窗基本資料
 {
    WORD height,width ;
    HDC hdc ;
    HGLRC hrc;
    HWND hwnd;
};
// function predefine
//------------------------
//init function 
void initGL();
int activeGL();
void key_function(short key);
int convert2(byte *);
// opengl function
void display();
int quads(double,double,double,double,double,double);
void _color_cube(int x,int y,int z,int r,int g,int b);
//GLvoid BuildFont(GLvoid);
GLvoid glPrint(const char *fmt, ...);
//GLvoid KillFont(GLvoid)	;
//windows function
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
HWND create_windows(const char *window_name ,int top,int left , int win_width ,int win_height) ;

//varible
GLuint	base;				// Base Display List For The Font Set
GLfloat	rot;				// Used To Rotate The Text
bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters
//===================================================================================================================
//gobal variable
//---------------------------------------------------------
window_info my_windows_info;
window_info my_windows_info2;
int map_on;
int map_3d_on;	
long wheel_speed=0;
int camera_x=0,camera_y=0,camera_z=0;
int rotate_x=0;
int record=0;//紀錄按鈕
short x_location=0,y_location=0;//紀錄滑鼠座標
int rnd_color[10][10]={0};
double shape_=13.5;
int base_color,base2_color;
int i=1;
int x_[2];
int y_[2];
int a,c;


double r[6]={255,0.0,0.0,255,128,255};
double g[6]={0.0,255,255,255,0.0,255};
double b[6]={0.0,0.0,255,0.0,128,255};
unsigned seed;

#define cubeon
//===================================================================================================================
           
int main()
{   seed = (unsigned)time(NULL);
	srand(seed);
	my_windows_info.height=	600;
    my_windows_info.width = 600;
	
    MSG msg ;
	//--------------------------------創造一個window出來
	my_windows_info.hwnd = create_windows("Soft Tissue Sugery on openGL",0,0,my_windows_info.width,my_windows_info.height) ; //create window
	
    if (!my_windows_info.hwnd)
	{
		printf("Big Error ,Create Window failed !");
		exit(1);
	}
	//--------------------------------準備openGL
	if(!activeGL())
	{
		printf("Big Error ,Active openGL failed !");
		exit(1);
	}
	//------------------------------- 初始化OPENGL
	initGL();
	
	ShowWindow(my_windows_info.hwnd, SW_SHOW);//全螢幕 SW_MAXIMIZE
	ShowWindow(my_windows_info2.hwnd, SW_SHOW);//全螢幕 SW_MAXIMIZE 
	//----------------------------------------------------------------------------------
	while(1)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //處理所有訊息
        {
            if (msg.message == WM_QUIT) break; //exit program
            //if(msg.message !=WM_KEYUP && msg.message !=WM_KEYDOWN) // pass keyboard's signals 
            {
                TranslateMessage (&msg) ; //處理得到的MSG
                DispatchMessage (&msg) ; //將得到的MSG丟給自身的WndProc
            }
        }
        
        Sleep(10);
        
		display();

		//-------------------------------------------
		SwapBuffers(my_windows_info.hdc);
		//SwapBuffers(my_windows_info2.hdc);
    }
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(my_windows_info.hrc);
    ReleaseDC(my_windows_info.hwnd,my_windows_info.hdc);
    DestroyWindow(my_windows_info.hwnd);

	
    return 0 ;
}
//------------------------------------------------------------------------------------
HWND create_windows(const char *window_name ,int top,int left , int win_width ,int win_height) //創造一個WIN32 OPENGL視窗
{
    WNDCLASS  windows_class ; //註冊函數的基底
    HINSTANCE hInstance =GetModuleHandle(NULL) ;

    windows_class.style = CS_OWNDC ;
    windows_class.lpfnWndProc =(WNDPROC)WndProc ;
    windows_class.cbClsExtra = 0 ;
    windows_class.cbWndExtra = 0 ;
    windows_class.hInstance = hInstance;
    windows_class.hIcon = LoadIcon (NULL, IDI_WINLOGO) ;
    windows_class.hCursor = LoadCursor (NULL, IDC_ARROW) ;
    windows_class.hbrBackground = NULL ;
    windows_class.lpszMenuName = NULL ;
    windows_class.lpszClassName = "OpenGL" ;

    if(!RegisterClass(&windows_class))
    {
        MessageBox(NULL,"RegeisterClass() failed","Error",MB_OK);
        return NULL;
    }
    my_windows_info.hwnd = CreateWindow("OpenGL",window_name,WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
                        left,top,win_height,win_width,NULL,NULL,hInstance,NULL);
    if(my_windows_info.hwnd==NULL)
    {
        MessageBox(NULL,"CreateWindow() failed","Error",MB_OK);
        return NULL;
    }
    my_windows_info.hdc =GetDC(my_windows_info.hwnd);
    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),
                                1,
                                PFD_DRAW_TO_WINDOW |
                                PFD_SUPPORT_OPENGL |
                                PFD_DOUBLEBUFFER,
                                PFD_TYPE_RGBA,
                                24,
                                0,0,0,0,0,0,
                                0,
                                0,
                                0,
                                0,0,0,0,
                                32,
                                0,
                                0,
                                PFD_MAIN_PLANE,
                                0,
                                0,0,0};
    int pf = ChoosePixelFormat(my_windows_info.hdc,&pfd ); //根據點結構選取點格式
    if(pf==0)
    {
        MessageBox(NULL,"ChoosePixelFormat() failed","Error",MB_OK);
        return NULL;
    }

    if(!SetPixelFormat(my_windows_info.hdc,pf,&pfd))//設定項點格式
    {
        MessageBox(NULL,"SetPixelFormat() failed","Error",MB_OK);
        return NULL;
    }
    return my_windows_info.hwnd;
}
//------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
		
		case WM_KEYDOWN  :
		{
			key_function(short(wParam));
					
				
		}
		return 0;
		
		
		case WM_MOUSEMOVE : //window mouse move events 滑鼠移動
        {  a=1;
        
        
			cout<<"WM_MOUSEMOVE"<<endl;
		 short xPos,yPos,fwKeys,zDelta;
			  fwKeys   =   LOWORD(wParam);   //滑鼠按鍵編號    
			  zDelta   =   (short)   HIWORD(wParam);//滾輪參數     
			 /*   wheel   rotation   */   
				xPos   =   (short)   LOWORD(lParam)-300+10; //滑鼠座標  X軸 
			 /*   horizontal   position   of   pointer   */   
				yPos   =   -1*((short)   HIWORD(lParam)-300+20);//滑鼠座標  X軸     
			/*   vertical   position   of   pointer   */
			cout<<"fwKeys:"<<fwKeys<<endl;
			cout<<"zDelta:"<<zDelta<<endl;
			cout<<"xPos"<<xPos<<endl;
			cout<<"yPos"<<yPos<<endl;
			x_location=xPos;
			y_location=yPos;
			if(a==1&&c==1){
			double shape_=13.5;
    
    for(int x=0;x<10;x++)
    {
		for(int y=0;y<10;y++)
		{
			if(fabs(x_location-(-255.5+x*(2*shape_+30)))<13.5&&fabs(y_location-(255.5-y*(2*shape_+30)))<13.5)
            {
               
                x_[i%2]=x;
                y_[i%2]=y;
               cout<<"rnd_color["<<x<<"]["<<y<<"]="<<rnd_color[x][y]<<endl;
                
               cout<<"x_["<<i%2<<"]="<<x_[i%2]<<endl;
               
               cout<<"y_["<<i%2<<"]="<<y_[i%2]<<endl<<endl;
               
               
               
               i++;
               
                if(fabs(x_[1]-x_[0])==1&&fabs(y_[1])==fabs(y_[0])&&rnd_color[x_[1]][y_[1]]==rnd_color[x_[0]][y_[0]])
	            {              
                       base_color=rnd_color[x_[1]][y_[1]];
                       base2_color=rnd_color[x_[0]][y_[0]];
                       cout<<"base_color="<<base_color<<endl;
                       cout<<"base2_color="<<base2_color<<endl<<endl;
                       rnd_color[x_[1]][y_[1]]=5;
                       rnd_color[x_[0]][y_[0]]=5;
                       
                       
                } 
                else if(fabs(x_[1]-x_[0])==1&&fabs(y_[0])==fabs(y_[1])&&rnd_color[x_[1]][y_[1]]==base_color)
	            {              
                       rnd_color[x_[1]][y_[1]]=5;
                       
                       
                       
                } 
                else if(fabs(x_[1]-x_[0])==1&&fabs(y_[0])==fabs(y_[1])&&rnd_color[x_[0]][y_[0]]==base2_color)
	            {              
                       rnd_color[x_[0]][y_[0]]=5;
                       
                       
                       
                } 
                
                                                                                                                                             
                else if(fabs(y_[1]-y_[0])==1&&fabs(x_[1])==fabs(x_[0])&&rnd_color[x_[1]][y_[1]]==rnd_color[x_[0]][y_[0]])
                {              base_color=rnd_color[x_[1]][y_[1]];
                               base2_color=rnd_color[x_[0]][y_[0]];                                                                                          
                               rnd_color[x_[1]][y_[1]]=5;
                               rnd_color[x_[0]][y_[0]]=5;
                               
                              
                }
                else if(fabs(y_[1]-y_[0])==1&&fabs(x_[0])==fabs(x_[1])&&rnd_color[x_[1]][y_[1]]==base_color)
	            {              
                       rnd_color[x_[1]][y_[1]]=5;
                       
                       
                       
                } 
                else if(fabs(y_[1]-y_[0])==1&&fabs(x_[0])==fabs(x_[1])&&rnd_color[x_[0]][y_[0]]==base2_color)
	            {              
                       rnd_color[x_[0]][y_[0]]=5;
                       
                       
                       
                } 
                
                
                
                
               
            }
		}
	
	}
            
        }
        return 0 ;
	}
        	case WM_LBUTTONUP:
		{	
			cout<<"WM_LBUTTONUP"<<endl;
			for(int m=0;m<10;m++)
                       {
                               for(int n=0;n<10;n++)
                               {
                                       if(rnd_color[m][n]==5)
                                       {
                                                             for(int k=0;k<10;k++)
                                                             {
                                                                     if((n-k)>0)
                                                                     {
                                                                     rnd_color[m][n-k]=rnd_color[m][n-k-1];
                                                                     a=0;
                       												c=0;
                                                                     }
                                                                     if(n-k==0)
                                                                     {
                                                                              rnd_color[m][n-k]=rand()%5;
                                                                              a=0;
                       															c=0;
                                                             }       } 
                                       }
                               }
                       }
		}
		return 0 ;
	
		case WM_RBUTTONDOWN:
		{
			cout<<"WM_RBUTTONDOWN"<<endl;
		}
		return 0 ;
		case WM_LBUTTONDOWN :
		{
		 cout<<"WM_LBUTTONDOWN"<<endl;
		 short xPos,yPos,fwKeys,zDelta;
			  fwKeys   =   LOWORD(wParam);   //滑鼠按鍵編號    
			  zDelta   =   (short)   HIWORD(wParam);//滾輪參數     
			 /*   wheel   rotation   */   
				xPos   =   (short)   LOWORD(lParam)-300+10; //滑鼠座標  X軸 
			 /*   horizontal   position   of   pointer   */   
				yPos   =   -1*((short)   HIWORD(lParam)-300+20);//滑鼠座標  X軸     
			/*   vertical   position   of   pointer   */
			cout<<"fwKeys:"<<fwKeys<<endl;
			cout<<"zDelta:"<<zDelta<<endl;
			cout<<"xPos"<<xPos<<endl;
			cout<<"yPos"<<yPos<<endl;
			x_location=xPos;
			y_location=yPos;
			
			//============================================================
			
			
			
			//============================================================
			c=1;
			
	
	
                                                      
            
			
			
			
			
			
			
			
			
		}
		return 0;
        case WM_MOUSEWHEEL : //滑鼠滾輪
        {	short xPos,yPos,fwKeys,zDelta;
			  fwKeys   =   LOWORD(wParam);   /*   key   flags   */     
			  zDelta   =   (short)   HIWORD(wParam);     
			 /*   wheel   rotation   */   
				xPos   =   (short)   LOWORD(lParam);     
			 /*   horizontal   position   of   pointer   */   
				yPos   =   (short)   HIWORD(lParam);     
			/*   vertical   position   of   pointer   */
			cout<<"fwKeys:"<<fwKeys<<endl;
			cout<<"zDelta:"<<zDelta<<endl;
			cout<<"xPos"<<xPos<<endl;
			cout<<"yPos"<<yPos<<endl;
			
			if(zDelta>0)
			{
				wheel_speed++;
			}
			else if(zDelta<0)
			{
				wheel_speed--;
			}
			cout<<wheel_speed<<endl;
        }
        return 0 ;
        case WM_SIZE : //reset mode display size 重設視窗大小
        {
            my_windows_info.width = LOWORD(lParam);            // X座標
            my_windows_info.height = HIWORD(lParam);
        }
        return 0 ;

        case WM_DESTROY:
        {
            PostQuitMessage (0) ;
        }
        return 0 ;

    }
    return DefWindowProc (hWnd, iMsg, wParam, lParam) ;
}
//-----------------------------------------------------------------------------------------------------------
int activeGL()
{
	my_windows_info.hdc =GetDC(my_windows_info.hwnd); //get dc
    my_windows_info.hrc = wglCreateContext(my_windows_info.hdc);
	if(my_windows_info.hdc ==NULL || my_windows_info.hrc ==NULL) return 0;
    wglMakeCurrent(my_windows_info.hdc,my_windows_info.hrc);
    UpdateWindow (my_windows_info.hwnd) ;
	return 1;
}
//-----------------------------------------------------------------------------------------------------------
void initGL()
{
	//=========================init value==========================
    glClearColor (0.0,0.0,0.0,1.0); //color for clear window
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SMOOTH);
	//BuildFont();
}
//-----------------------------------------------------------------------------------------------------------
void display() //opengl show object and all graph
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective                            // 設置透視圖
        (45,                            // 透視角設置? 45 度,在Y方向上以角度?單位的視野
        my_windows_info.width/my_windows_info.height,    // 窗口的寬與高比
        1.0f,                                // 視野透視深度:近點1.0f
        4000.0f                            // 視野透視深度:始點0.1f遠點1000.0f
        );

	


		
//========================繪圖空間定義===========================================================
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(-1*(my_windows_info.width/2),my_windows_info.width/2,-1*(my_windows_info.height/2),my_windows_info.height/2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	static float value=0;
//=======================GL畫圖筆記==================================================================
/*

//=======================POINT(點)===============================================================
    glPointSize(10.0);   //設定大小 
    glColor3f(1.0,1.0,1.0); //設定顏色 (混色 red + green+ blue)
    glBegin(GL_POINTS);  //         
    glVertex3f(0,0,0);//設定位置
    glEnd();
//======================Line(線)==============================================================
    glLineWidth(1); //線條寬度
    glColor3f(1.0,1.0,1.0); //設定顏色 (混色 red + green+ blue)
    glBegin(GL_LINES);
    glVertex3f(0,0,0); //起始點 
    glVertex3f(200,0,0); //結束點 
    glEnd();
//=====================triangles(三角形)=====================================================================
    glColor3f(1.0,1.0,1.0); //設定顏色 (混色 red + green+ blue)
    glBegin(GL_TRIANGLES); 
    glVertex3f(0,50,0.0f); 
    glVertex3f(-50,-50.0, 0.0f); 
    glVertex3f(50,-50, 0.0f); 
    glEnd(); 
//=====================GL_QUADS(正方形)=====================================================================
    glBegin(GL_QUADS); 
    glVertex3f(-50,50.0f, 0.0f);          
    glVertex3f(50.0,50.0, 0.0f);          
    glVertex3f(50.0f,-50.0f, 0.0f);          
    glVertex3f(-50.0f,-50.0f, 0.0f);  
    glEnd(); 
*/

//==============================OPENGL 基本圖形繪製==============================================

    /*
    static int x_point=100,y_point=-50;
    static int x_vector=1,y_vector=1;
    if(x_point>180 || x_point<-180)
    {
        x_vector=x_vector*-1;
        glColor3f(1.0,1.0,1.0); //設定顏色
    }
    if(y_point>180 || y_point<-180)
    {
        y_vector=y_vector*-1;
        glColor3f(0.0,1.0,1.0); //設定顏色
    }
    x_point=x_point+x_vector;
    y_point=y_point+y_vector;
   
    glBegin(GL_QUADS);                                
    glVertex3f(-25+x_point,25.0f+y_point, 0.0f);          
    glVertex3f(25.0+x_point,25.0+y_point, 0.0f);          
    glVertex3f(25.0f+x_point,-25.0f+y_point, 0.0f);          
    glVertex3f(-25.0f+x_point,-25.0f+y_point, 0.0f);          
    glEnd();
    */
    
    static int count=0;
    
    if(count==0)
	{ 
		
	    for(int x=0;x<10;x++)
	    {
			for(int y=0;y<10;y++)
			{  
				rnd_color[x][y]=rand()%5  ;
			}
		
		}
		count=1;
	}
    
    
    double shape_=13.5;
    for(int x=0;x<10;x++)
    {
		for(int y=0;y<10;y++)
		{
			quads(-255.5+x*(2*shape_+30),255.5-y*(2*shape_+30),shape_,r[rnd_color[x][y]],g[rnd_color[x][y]],b[rnd_color[x][y]])	;
			
		}
	
	}
    
    quads(x_location,y_location,5,1,1,1);
                       
    } 
    
//==============================OPENGL 基本圖形繪製==============================================

/************************************************************************************************/
/************************************************************************************************/
	


int quads(double x,double y,double size,double r,double g,double b)
{   glColor3f(r,g,b);
	glBegin(GL_QUADS);                      // Draw A Quad
        glVertex3f(-1.0f*size+x,1.0f*size+y, 0.0f);          // Left And Up 1 Unit (Top Left)
        glVertex3f(1.0f*size+x,1.0f*size+y, 0.0f);          // Right And Up 1 Unit (Top Right)
        glVertex3f(1.0f*size+x,-1.0f*size+y, 0.0f);          // Right And Down One Unit (Bottom Right)
        glVertex3f(-1.0f*size+x,-1.0f*size+y, 0.0f);
    glEnd();
	return 0;
}




void key_function(short key)
{
	if(key==37)
	{
		
		camera_x+=5;
	}
	if(key==38)
	{
		
		camera_y+=5;
	}
	if(key==39)
	{
		
		camera_x-=5;
	}
	if(key==40)
	{
		
		camera_y-=5;
	}
	if(key==112)
	{
		
		map_on++;
		
	}
	if(key==113)
	{
		
		map_3d_on++;
		
	}
	if(key==65)
	{
		
		rotate_x++;
		cout<<rotate_x;
	}
	if(key==68)
	{
		
		rotate_x--;
		cout<<rotate_x;
	}
	if(key==32)
	{
	   record++;
	}
	cout<<key;
}

void _color_cube(int x ,int y,int z,int r,int g,int b)
{
	glBegin(GL_QUADS);                  // Start Drawing The Cube	
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);          // Set The Color To Green
	//======================================================================
	glVertex3f( 1.0f+x, 1.0f+y,-1.0f+z);          // Top Right Of The Quad (Top)
	glVertex3f(-1.0f+x, 1.0f+y,-1.0f+z);          // Top Left Of The Quad (Top)
	glVertex3f(-1.0f+x, 1.0f+y, 1.0f+z);          // Bottom Left Of The Quad (Top)
	glVertex3f( 1.0f+x, 1.0f+y, 1.0f+z);          // Bottom Right Of The Quad (Top)
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);           // Set The Color To Orange
	//======================================================================
	glVertex3f( 1.0f+x,-1.0f+y, 1.0f+z);          // Top Right Of The Quad (Bottom)
	glVertex3f(-1.0f+x,-1.0f+y, 1.0f+z);          // Top Left Of The Quad (Bottom)
	glVertex3f(-1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Left Of The Quad (Bottom)
	glVertex3f( 1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Right Of The Quad (Bottom)
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);           // Set The Color To Red
	//======================================================================
	glVertex3f( 1.0f+x, 1.0f+y, 1.0f+z);          // Top Right Of The Quad (Front)
	glVertex3f(-1.0f+x, 1.0f+y, 1.0f+z);          // Top Left Of The Quad (Front)
	glVertex3f(-1.0f+x,-1.0f+y, 1.0f+z);          // Bottom Left Of The Quad (Front)
	glVertex3f( 1.0f+x,-1.0f+y, 1.0f+z);          // Bottom Right Of The Quad (Front)
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);           // Set The Color To Orange
	//======================================================================
	glVertex3f( 1.0f+x,-1.0f+y, 1.0f+z);          // Top Right Of The Quad (Bottom)
	glVertex3f(-1.0f+x,-1.0f+y, 1.0f+z);          // Top Left Of The Quad (Bottom)
	glVertex3f(-1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Left Of The Quad (Bottom)
	glVertex3f( 1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Right Of The Quad (Bottom)
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);         // Set The Color To Yellow
	//======================================================================
	glVertex3f( 1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Left Of The Quad (Back)
	glVertex3f(-1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Right Of The Quad (Back)
	glVertex3f(-1.0f+x, 1.0f+y,-1.0f+z);          // Top Right Of The Quad (Back)
	glVertex3f( 1.0f+x, 1.0f+y,-1.0f+z);          // Top Left Of The Quad (Back)
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);           // Set The Color To Blue
	//======================================================================
	glVertex3f(-1.0f+x, 1.0f+y, 1.0f+z);          // Top Right Of The Quad (Left)
	glVertex3f(-1.0f+x, 1.0f+y,-1.0f+z);          // Top Left Of The Quad (Left)
	glVertex3f(-1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Left Of The Quad (Left)
	glVertex3f(-1.0f+x,-1.0f+y, 1.0f+z);          // Bottom Right Of The Quad (Left)
	//======================================================================
	glColor3f(0.0f+r,0.0f+g,0.0f+b);           // Set The Color To Violet
	//======================================================================
	glVertex3f( 1.0f+x, 1.0f+y,-1.0f+z);          // Top Right Of The Quad (Right)
	glVertex3f( 1.0f+x, 1.0f+y, 1.0f+z);          // Top Left Of The Quad (Right)
	glVertex3f( 1.0f+x,-1.0f+y, 1.0f+z);          // Bottom Left Of The Quad (Right)
	glVertex3f( 1.0f+x,-1.0f+y,-1.0f+z);          // Bottom Right Of The Quad (Right)
glEnd();             
}
/*
GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID

	base = glGenLists(256);								// Storage For 256 Characters

	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Comic Sans MS");				// Font Name
	my_windows_info.hdc =GetDC(my_windows_info.hwnd); //get dc

	SelectObject(my_windows_info.hdc, font);							// Selects The Font We Created

	wglUseFontOutlines(	my_windows_info.hdc,							// Select The Current DC
						0,								// Starting Character
						255,							// Number Of Display Lists To Build
						base,							// Starting Display Lists
						0.0f,							// Deviation From The True Outlines
						0.2f,							// Font Thickness In The Z Direction
						WGL_FONT_POLYGONS,				// Use Polygons, Not Lines
						gmf);							// Address Of Buffer To Recieve Data
}
*/
/*
GLvoid KillFont(GLvoid)									// Delete The Font
{
  glDeleteLists(base, 256);								// Delete All 256 Characters
}
*/

GLvoid glPrint(const char *fmt, ...)					// Custom GL "Print" Routine
{
	float		length=0;								// Used To Find The Length Of The Text
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	for (unsigned int loop=0;loop<(strlen(text));loop++)	// Loop To Find Text Length
	{
		length+=gmf[text[loop]].gmfCellIncX;			// Increase Length By Each Characters Width
	}

	glTranslatef(-length/2,0.0f,0.0f);					// Center Our Text On The Screen

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base);									// Sets The Base Character to 0
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}
