/*
CPE/CSC 474 Lab base code Eckhardt/Dahl
based on CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>



#include "GLSL.h"
#include "Program.h"
#include "WindowManager.h"
#include "Shape.h"
#include "line.h"
#include "skmesh.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// assimp
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/vector3.h"
#include "assimp/scene.h"
#include <assimp/mesh.h>

int realspeed = 0;

#include <math.h>
#include <algorithm>

float cosint(float t){
    return 1 - (cos(t*3.1415)+1.0f)/2.0f;
}



using namespace std;
using namespace glm;
using namespace Assimp;
mat4 linint_between_two_orientations(mat4 m1, mat4 m2, float t);

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
mat4 linint_between_two_orientations(mat4 m1, mat4 m2, float t)
    {
    quat q1, q2;
    
    q1 = quat(m1);
    q2 = quat(m2);
    quat qt = slerp(q1, q2, t); //<---
    qt = normalize(qt);
    mat4 mt = mat4(qt);
    //mt = transpose(mt);         //<---
    return mt;
    }

mat4 create_orientations (vec3 ez_aka_lookto_1, vec3 ey_aka_up_1) {
    mat4 m1;
    quat q1;
    vec3 ex, ey, ez;
    ey = ey_aka_up_1;
    ez = ez_aka_lookto_1;
    ex = cross(ey, ez);
    m1[0][0] = ex.x;        m1[0][1] = ex.y;        m1[0][2] = ex.z;        m1[0][3] = 0;
    m1[1][0] = ey.x;        m1[1][1] = ey.y;        m1[1][2] = ey.z;        m1[1][3] = 0;
    m1[2][0] = ez.x;        m1[2][1] = ez.y;        m1[2][2] = ez.z;        m1[2][3] = 0;
    m1[3][0] = 0;            m1[3][1] = 0;            m1[3][2] = 0;            m1[3][3] = 1.0f;
    
    return m1;
}

class camera
{
public:
    glm::vec3 pos, rot;
    int w, a, s, d, q, e, z, c;
    bool slow;
    camera()
    {
        w = a = s = d = q = e = z = c = 0;
        pos = glm::vec3(40.0f, -5.0f, 20.0f);
        rot = glm::vec3(0, 1.5f, 0);
        slow = false;
    }
    glm::mat4 process(double ftime)
    {
        float speed = 0;
        float multiple = 0;
        
        if(slow)
        {
            multiple = 0.05;
        }
        else{
            multiple = 3;
        }

        float fwdspeed = 20;
        if (realspeed)
            fwdspeed = 8;

        if (w == 1)
        {
            speed = fwdspeed*ftime;
        }
        else if (s == 1)
        {
            speed = -fwdspeed*ftime;
        }
        float yangle=0;
        if (a == 1)
            yangle = -multiple*ftime;
        else if(d==1)
            yangle = multiple*ftime;
        rot.y += yangle;
        float zangle = 0;
        if (q == 1)
            zangle = -3 * ftime;
        else if (e == 1)
            zangle = 3 * ftime;
        rot.z += zangle;
        float xangle = 0;
        if (z == 1)
            xangle = -0.3 * ftime;
        else if (c == 1)
            xangle = 0.3 * ftime;
        rot.x += xangle;

        glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
        glm::mat4 Rz = glm::rotate(glm::mat4(1), rot.z, glm::vec3(0, 0, 1));
        glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
        glm::vec4 dir = glm::vec4(0, 0, speed,1);
        R = Rz * Rx  * R;
        dir = dir*R;
        pos += glm::vec3(dir.x, dir.y, dir.z);
        glm::mat4 T = glm::translate(glm::mat4(1), pos);
        return R*T;
    }
    void get_dirpos(vec3 &up,vec3 &dir,vec3 &position)
        {
        position = pos;
        glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
        glm::mat4 Rz = glm::rotate(glm::mat4(1), rot.z, glm::vec3(0, 0, 1));
        glm::mat4 Rx = glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
        glm::vec4 dir4 = glm::vec4(0, 0, 1, 0);
        R = Rz * Rx  * R;
        dir4 = dir4*R;
        dir = vec3(dir4);
        glm::vec4 up4 = glm::vec4(0, 1, 0, 0);
        up4 = R*vec4(0, 1, 0, 0);
        up4 = vec4(0, 1, 0, 0)*R;
        up = vec3(up4);
        }
    
};

camera mycam;

//class camera
//{
//public:
//	glm::vec3 pos, rot;
//	int w, a, s, d;
//	camera()
//	{
//		w = a = s = d = 0;
//		pos = glm::vec3(40.0f, -5.0f, 20.0f);
//		rot = glm::vec3(0, 1.5f, 0);
//	}
//
//	glm::mat4 process(double ftime)
//	{
//		double speed = 0.0f;
//
//		if (w == 1)
//		{
//			speed = 10*ftime;
//		}
//		else if (s == 1)
//		{
//			speed = -10*ftime;
//		}
//        double yangle=0;
//		if (a == 1)
//			yangle = -3*ftime;
//		else if(d==1)
//			yangle = 3*ftime;
//		rot.y += (float)yangle;
//		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
//		glm::vec4 dir = glm::vec4(0, 0, speed, 1);
//		dir = dir*R;
//		pos += glm::vec3(dir.x, dir.y, dir.z);
//		glm::mat4 T = glm::translate(glm::mat4(1), pos);
//		return R*T;
//	}
//};
//
//camera mycam;

class Application : public EventCallbacks
{
public:
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> psky, skinProg, skinProg2;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexNormDBox, VertexTexBox, IndexBufferIDBox;

	//texture data
	GLuint Texture;
	GLuint Texture2;
    
    vector<vec3> line;
    
    //cam
    Line linerendercam;
    Line smoothrendercam;
    vector<vec3> linecam;
    vector<vec3> splinepointscam;
    
    //orientations cam
    vector<vec3> eycam;
    vector<vec3> ezcam;
    vector<mat4> marrcam;

	// skinnedMesh
	SkinnedMesh skmesh, skmesh2, skmeshf2;

	// textures
	shared_ptr<SmartTexture> skyTex, skyTexChange, meshTex, meshTex2;

	// shapes
	shared_ptr<Shape> skyShape;
	
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
        if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        {
            mycam.q = 1;
        }
        if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
        {
            mycam.q = 0;
        }
        if (key == GLFW_KEY_E && action == GLFW_PRESS)
        {
            mycam.e = 1;
        }
        if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        {
            mycam.e = 0;
        }
        if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        {
            mycam.z = 1;
        }
        if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
        {
            mycam.z = 0;
        }
        if (key == GLFW_KEY_C && action == GLFW_PRESS)
        {
            mycam.c = 1;
        }
        if (key == GLFW_KEY_C && action == GLFW_RELEASE)
        {
            mycam.c = 0;
        }
        
        
        

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			//mycam.pos = vec3(mycam.pos.x, mycam.pos.y-0.1, mycam.pos.z);
			//skmesh.SetNextAnimation(1);
            mycam.slow = !mycam.slow;
		}
        
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
            {
            vec3 dir,pos,up;
            mycam.get_dirpos(up, dir, pos);
            cout << "point position:" << pos.x << "," << pos.y<< "," << pos.z << endl;
            cout << "linecam.push_back(vec3(" << pos.x << "," << pos.y<< "," << pos.z << "));" << endl;
                
            
            cout << "Zbase:" << dir.x << "," << dir.y << "," << dir.z << endl;
            cout << "Ybase:" << up.x << "," << up.y << "," << up.z << endl;
            }
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			std::cout << "Pos X " << posX <<  " Pos Y " << posY << std::endl;

			//change this to be the points converted to WORLD
			//THIS IS BROKEN< YOU GET TO FIX IT - yay!
			newPt[0] = 0;
			newPt[1] = 0;

			std::cout << "converted:" << newPt[0] << " " << newPt[1] << std::endl;
			glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);
			//update the vertex array with the updated points
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float)*2, newPt);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
	/*Note that any gl calls must always happen after a GL state is initialized */
	
	
	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom(const std::string& resourceDirectory)
	{
		if (!skmesh.LoadMesh(resourceDirectory + "/sleeping_beauty_male.fbx")) {
			printf("Mesh load failed\n");
			return;
			}
		

		auto strMesh = resourceDirectory + "/test_mat.jpg";
		meshTex = SmartTexture::loadTexture(strMesh, true);
		if (!meshTex)
			cerr << "error: texture " << meshTex << " not found" << endl;


		
		//2nd fbx
		if (!skmesh2.LoadMesh(resourceDirectory + "/sleeping_beauty_female.fbx")) {
			printf("Mesh load failed\n");
			return;
			}

		if (!skmeshf2.LoadMesh(resourceDirectory + "/modern_intro_standing.fbx")) {
			printf("Mesh load failed\n");
			return;
			}

		auto strMesh2 = resourceDirectory + "/female_mat.jpg";
		meshTex2 = SmartTexture::loadTexture(strMesh2, true);
		if (!meshTex2)
			cerr << "error: texture " << meshTex2 << " not found" << endl;

		/*if (!skmesh.LoadMesh(resourceDirectory + "/Campfire.fbx")) {
			printf("Mesh load failed\n");
			return;
			}*/

		// Initialize mesh.
		skyShape = make_shared<Shape>();
		skyShape->loadMesh(resourceDirectory + "/sphere.obj");
		skyShape->resize();
		skyShape->init();

		// sky texture
		auto strSky = resourceDirectory + "/sky_pink.jpg";
		skyTex = SmartTexture::loadTexture(strSky, true);
		if (!skyTex)
			cerr << "error: texture " << strSky << " not found" << endl;
        
        auto strSkyChange = resourceDirectory + "/sky_dark.jpg";
        skyTexChange = SmartTexture::loadTexture(strSkyChange, true);
        if (!skyTexChange)
            cerr << "error: texture " << strSkyChange << " not found" << endl;
        
        
        //CAM_____________________________________________
        smoothrendercam.init();
        linerendercam.init();
        
        linecam.push_back(vec3(14.1174,-2.16501,22.8123));
//        linecam.push_back(vec3(4.29011,-0.883828,57.0041));
        ezcam.push_back(vec3(-0.953856,0.252053,0.163179));
        eycam.push_back(vec3(0.256267,0.966593,0.00495671));

        linecam.push_back(vec3(1.9445,-2.46303,7.19052));
//        linecam.push_back(vec3(0.989476,-0.940278,46.5096));
        ezcam.push_back(vec3(-0.385047,0.290315,0.876046));
        eycam.push_back(vec3(0.160713,0.955824,-0.246114));

        linecam.push_back(vec3(-13.6491,-3.77908,8.74638));
//        linecam.push_back(vec3(-9.0083,-1.11127,14.7213));
        ezcam.push_back(vec3(0.4084,0.173023,0.896255));
        eycam.push_back(vec3(-0.0278932,0.983778,-0.177209));

//        linecam.push_back(vec3(-28.3867,-2.26815,30.5698));
//        ezcam.push_back(vec3(0.947629,0.246539,-0.20302));
//        eycam.push_back(vec3(-0.250866,0.968011,0.0045556));

        linecam.push_back(vec3(-13.0827,-3.26426,47.7247));
        ezcam.push_back(vec3(0.426807,0.177986,-0.886655));
        eycam.push_back(vec3(-0.120447,0.982894,0.139325));

        linecam.push_back(vec3(18.2616,-2.91051,35.2271));
        ezcam.push_back(vec3(-0.883206,0.128517,-0.451033));
        eycam.push_back(vec3(0.0924482,0.99056,0.101218));

   
        
        linerendercam.re_init_line(line);
        
        
        spline(splinepointscam, linecam, 30, 1.0);
        smoothrendercam.re_init_line(splinepointscam);
        
        for (int i = 0; i < eycam.size(); i++) {
            marrcam.push_back(create_orientations(eycam[i], ezcam[i]));
            
        }
        
        
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		// Initialize the GLSL program.
		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addUniform("tex");
		psky->addUniform("camPos");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertNor");
		psky->addAttribute("vertTex");

		skinProg = std::make_shared<Program>();
		skinProg->setVerbose(true);
		skinProg->setShaderNames(resourceDirectory + "/skinning_vert_male.glsl", resourceDirectory + "/skinning_frag_male.glsl");
		if (!skinProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		
		skinProg->addUniform("P");
		skinProg->addUniform("V");
		skinProg->addUniform("M");
		skinProg->addUniform("tex");
		skinProg->addUniform("camPos");
		skinProg->addAttribute("vertPos");
		skinProg->addAttribute("vertNor");
		skinProg->addAttribute("vertTex");
		skinProg->addAttribute("BoneIDs");
		skinProg->addAttribute("Weights");
        
        skinProg->addUniform("time");
        skinProg->addUniform("start_interp");


		skinProg2 = std::make_shared<Program>();
		skinProg2->setVerbose(true);
		skinProg2->setShaderNames(resourceDirectory + "/skinning_vert_male.glsl", resourceDirectory + "/skinning_frag_male.glsl");
		if (!skinProg2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		
		skinProg2->addUniform("P");
		skinProg2->addUniform("V");
		skinProg2->addUniform("M");
		skinProg2->addUniform("tex");
		skinProg2->addUniform("camPos");
		skinProg2->addAttribute("vertPos");
		skinProg2->addAttribute("vertNor");
		skinProg2->addAttribute("vertTex");
		skinProg2->addAttribute("BoneIDs");
		skinProg2->addAttribute("Weights");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		double frametime = get_last_elapsed_time();
		//SET START TIME
		static double totaltime = 45;
		totaltime += frametime;

		//logic for interpolating between translation fixes
		double tenth_time = totaltime;
        int ittime = (int)tenth_time;
        
        //interpolation
        float t = (float)tenth_time - (float)ittime;

		//previous position vectore
		glm::vec3 previous_pos_female;
		glm::vec3 final_pos_female;
		glm::vec3 current_pos_female;

		glm::vec3 previous_pos_male;
		glm::vec3 final_pos_male;
		glm::vec3 current_pos_male;

		bool change_t = false;

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width/(float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now
		
		glm::mat4 V, Vi, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
        
        Vi = glm::inverse(V);
        
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);		
		if (width < height)
			{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect,  1.0f / aspect, -2.0f, 100.0f);
			}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		auto sangle = -3.1415926f / 2.0f;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = -mycam.pos;
        
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky  * RotateXSky * SSky;
        
        
        
        
//        glm::vec3 linecolor = glm::vec3(1, 0, 0);
//        linerendercam.draw(P, V, linecolor);

		// Draw the sky using GLSL.
		psky->bind();
		GLuint texLoc = glGetUniformLocation(psky->pid, "tex");
        
        //bind pink skybox
        if (totaltime < 52)
        {
            skyTex->bind(texLoc);
            
        }
        else //bind dark skybox
        {
            skyTexChange->bind(texLoc);
            float itenthtimecam = ittime /3;
            
            glm::vec3 Ac = splinepointscam[ittime];
            glm::vec3 Bc = splinepointscam[ittime+1];
                    
            mycam.pos = (Ac * (1 - t)) + (Bc * t);
        }
			

		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("camPos"), 1, &mycam.pos[0]);

		glDisable(GL_DEPTH_TEST);
		skyShape->draw(psky, false);
		glEnable(GL_DEPTH_TEST);
        
        //bind pink skybox
        if (totaltime < 52)
        {
            skyTex->unbind();
        }
        else //bind dark skybox
        {
            skyTexChange->unbind();
        }
        
		
		psky->unbind();

		// draw the skinned mesh
		//mesh for male sleeping beauty. 
		//Need to start _ seconds later	

		//print out the exact time
		cout<< totaltime <<endl;

        // -------------------------------------------------------------------------------------
        
        float start_interp = 0;
		//start the male animation
		if (totaltime > 4.5 && totaltime < 51.0){
            
			previous_pos_male = vec3(0, 0, 10);
			current_pos_male = vec3(0, 0, 10);
			skinProg->bind();
			texLoc = glGetUniformLocation(skinProg->pid, "tex");
			sangle = -3.1415926f / 2.0f;
			//glm::mat4 Trans = glm::translate(glm::mat4(1.0f), vec3(0, 0, 10));
			glm::mat4 RotX = glm::rotate(glm::mat4(1.0f), sangle, vec3(1, 0, 0));
			//need to rotate them 45 degrees counter clockwise to have them face the front
            
            if (totaltime > 41){
                start_interp = 10.f; // we want to change opacity
                
//                static double blendtime = 0;
//                blendtime += frametime;
//
//                //logic for interpolating between translation fixes
//                double tenth_time_bl = blendtime;
//                int ittime_bl = (int)tenth_time_bl;
//
//                //interpolation
//                float t_bl = (float)tenth_time_bl - (float)ittime_bl;
                
            }
			
			if (totaltime > 42.5){
				if (totaltime < 42.8){
					change_t=true;
				}
				//translate him a bit to the left
					
				previous_pos_male = vec3(0, 0, 10);
				current_pos_male = vec3(0, 0, 9);
				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(1, 1.8, -33));
			}
			//try comparing the vectors and if they are equal 
			if(change_t){
				final_pos_male = previous_pos_male * (1-t) + current_pos_male * t;
				if (!(final_pos_male == current_pos_male)){
					change_t=false;
				}
			}
			else{
					
				final_pos_male = current_pos_male;
			}
			
			
			glm::mat4 Trans = glm::translate(glm::mat4(1.0f), final_pos_male);

			glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.0005f, 0.0005f, 0.0005f));
			M = Trans * Scale;
			meshTex->bind(texLoc);

			glUniform3fv(skinProg->getUniform("camPos"), 1, &mycam.pos[0]);
			glUniformMatrix4fv(skinProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(skinProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(skinProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            
            glUniform1fv(skinProg->getUniform("start_interp"), 1, &start_interp);
            glUniform1fv(skinProg->getUniform("time"), 1, &t);
			skmesh.setBoneTransformations(skinProg->pid, totaltime - 4.5f);
			skmesh.Render(texLoc);
			skinProg->unbind();
		}


        // -------------------------------------------------------------------------------------
		// draw the second skinned mesh
		//this is female sleeping beauty waltz
		skinProg2->bind();
		texLoc = glGetUniformLocation(skinProg2->pid, "tex");
		previous_pos_female = vec3(0, 0, -30);
		current_pos_female = vec3(0, 0, -30);
		glm::mat4 RotY = glm::rotate(glm::mat4(1.0f), sangle, vec3(0, 1, 0));
		glm::mat4 Trans2 = glm::translate(glm::mat4(1.0f), previous_pos_female);
		glm::mat4 RotX2 = glm::rotate(glm::mat4(1.0f), sangle, vec3(1, 0, 0));
		glm::mat4 Scale2 = glm::scale(glm::mat4(1.0f), glm::vec3(0.0005f, 0.0005f, 0.0005f));
        float start_interpf = 0.f;
        if (totaltime > 41){
            start_interpf = 10.f; // we want to change opacity
            
        }
        
		if (totaltime < 8.0 || totaltime > 11.0 && totaltime < 52.3 ){
			previous_pos_female = vec3(0, 0, -30);
			current_pos_female = vec3(0, 0, -30);
			
			sangle = 3.1415926f/1.3f;
			
			
			
			//-x is toward the camera
			//+x is away from the camera

			//+z is to the right
			//-z is to the left
			//-30 z is 0
            

			//translate differently after certain amount of time passes (about 20 seconds)
			if (totaltime > 20.0){
				if (totaltime < 21.0){
					change_t=true;
				}
				current_pos_female = vec3(-1, -0.2, -28);

				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(-1, -0.2, -28));
			}
			//translate differently after certain amount of time passes (25 sec) move back to the left
			if (totaltime > 25.0){
				if (totaltime < 26.0){
					change_t=true;
				}
				previous_pos_female = vec3(-1, -0.2, -28);
				current_pos_female = vec3(-1, -0.2, -30);

				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(-1, -0.2, -30));
			}
			if (totaltime > 37.0){
				if (totaltime < 38.0){
					change_t=true;
				}
				previous_pos_female = vec3(-1, -0.2, -30);
				current_pos_female = vec3(1, -0.2, -33);

				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(1, -0.2, -33));
			}
			
			//fix the lift at 40 seconds
			if (totaltime > 40.0){
				if (totaltime < 40.3){
					change_t=true;
					tenth_time = totaltime*3.0;
        			ittime = (int)tenth_time;
        
       				 //interpolation
        			t = (float)tenth_time - (float)ittime;
				}
					
				previous_pos_female = vec3(1, -0.2, -33);
				current_pos_female = vec3(1.5, 2.0, -32);
				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(1, 1.8, -33));
			}
			
			if (totaltime > 42.5){
				if (totaltime < 42.8){
					change_t=true;
					tenth_time = totaltime*3.0;
        			ittime = (int)tenth_time;
        
       				 //interpolation
        			t = (float)tenth_time - (float)ittime;
				}
					
				previous_pos_female = vec3(1.5, 2.0, -32);
				current_pos_female = vec3(1.5, 4.0, -31);
				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(1, 1.8, -33));
			}
			//make her drop
			if (totaltime > 51.3){
				if (totaltime < 51.6){
					change_t=true;
					tenth_time = totaltime*3.0;
        			ittime = (int)tenth_time;
        
       				 //interpolation
        			t = (float)tenth_time - (float)ittime;
				}
					
				previous_pos_female = vec3(1.5, 4.0, -31);
				current_pos_female = vec3(1.5, -8.0, -31);
				//Trans2 = glm::translate(glm::mat4(1.0f), vec3(1, 1.8, -33));
			}

			//try comparing the vectors and if they are equal 
			if(change_t){
				final_pos_female = previous_pos_female * (1-t) + current_pos_female * t;
				if (!(final_pos_female == current_pos_female)){
					change_t=false;
				}
			}
			else{
					
				final_pos_female = current_pos_female;
			}
			
			
			Trans2 = glm::translate(glm::mat4(1.0f), final_pos_female);
			
	
		}

		if (totaltime > 52.3){

			Trans2 = glm::translate(glm::mat4(1.0f), vec3(1.5, -10.0, -27));
		}
		M = Trans2 *Scale2;
		meshTex2->bind(texLoc);
        
        float ten_rand = totaltime*3.0;
        int rand_ittime = (int)ten_rand;

            //interpolation
        float t_rand = (float)ten_rand - (float)rand_ittime;

		glUniform3fv(skinProg2->getUniform("camPos"), 1, &mycam.pos[0]);
		glUniformMatrix4fv(skinProg2->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(skinProg2->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(skinProg2->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        
        glUniform1fv(skinProg->getUniform("start_interp"), 1, &start_interpf);
        glUniform1fv(skinProg->getUniform("time"), 1, &t);
        
        
		if (totaltime < 8.0 || totaltime > 11.0 && totaltime < 52.3 ){
			skmesh2.setBoneTransformations(skinProg2->pid, totaltime);
			skmesh2.Render(texLoc);
		}
		else if (totaltime > 52.5){

//			skmeshf2.setBoneTransformations(skinProg2->pid, (totaltime + 2.5));
			skmeshf2.Render(texLoc);
            start_interpf = 19.f;
		}
		else if (totaltime > 52.3){

			skmeshf2.setBoneTransformations(skinProg2->pid, (totaltime + 3.0));
			skmeshf2.Render(texLoc);
		}
		
		else{
		}
		skinProg2->unbind();
        
	
	}
};

//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	std::string missingTexture = "missing.png";
	
	SkinnedMesh::setResourceDir(resourceDir);
	SkinnedMesh::setDefaultTexture(missingTexture);
	
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}

