#include "BasicSceneRenderer.h"
#include "Image.h"
#include "Prefabs.h"
#include "Arrow.h"
#include "AABB.h"

#include <iostream>
#include <algorithm>

BasicSceneRenderer::BasicSceneRenderer()
    : mLightingModel(BLINN_PHONG_PER_FRAGMENT_MULTI_LIGHT)
    , mCamera(NULL)
    , mProjMatrix(1.0f)
    , mActiveEntityIndex(0)
    , mDbgProgram(NULL)
    , mAxes(NULL)
    , mVisualizePointLights(false)
{
}

Entity* active;

Entity* ray;
Mesh* rayMesh;
Entity* smallCube;
Entity* wireframeCube;

Arrow* arrow;

void BasicSceneRenderer::initialize()
{
    // print usage instructions
    std::cout << "Usage:" << std::endl;
    std::cout << "  Camera control:           WASD + Mouse" << std::endl;
    std::cout << "  Rotate active entity:     Arrow keys" << std::endl;
    std::cout << "  Reset entity orientation: R" << std::endl;
    std::cout << "  Translate active entity:  IJKL (world space)" << std::endl;
    std::cout << "  Translate active entity:  TFGH (local space)" << std::endl;
    std::cout << "  Cycle active entity:      X/Z" << std::endl;
    std::cout << "  Toggle point light vis.:  Tab" << std::endl;

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enable blending (needed for textures with alpha channel)
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mPrograms.resize(NUM_LIGHTING_MODELS);

    mPrograms[PER_VERTEX_DIR_LIGHT] = new ShaderProgram("shaders/PerVertexDirLight-vs.glsl",
                                                                "shaders/PerVertexDirLight-fs.glsl");
    
    mPrograms[BLINN_PHONG_PER_FRAGMENT_DIR_LIGHT] = new ShaderProgram("shaders/BlinnPhongPerFragment-vs.glsl",
                                                                      "shaders/BlinnPhongPerFragmentDirLight-fs.glsl");

    mPrograms[BLINN_PHONG_PER_FRAGMENT_POINT_LIGHT] = new ShaderProgram("shaders/BlinnPhongPerFragment-vs.glsl",
                                                                        "shaders/BlinnPhongPerFragmentPointLight-fs.glsl");

    mPrograms[BLINN_PHONG_PER_FRAGMENT_MULTI_LIGHT] = new ShaderProgram("shaders/BlinnPhongPerFragment-vs.glsl",
                                                                        "shaders/BlinnPhongPerFragmentMultiLight-fs.glsl");

	glLineWidth(2.0f);



    //
    // Create meshes
    //

    mMeshes.push_back(CreateTexturedCube(1));
    mMeshes.push_back(CreateChunkyTexturedCylinder(0.5f, 1, 8));
    mMeshes.push_back(CreateSmoothTexturedCylinder(0.5f, 1, 15));

    float roomWidth = 32;
    float roomHeight = 24;
    float roomDepth = 52;
    float roomTilesPerUnit = 0.25f;

    // front and back walls
    Mesh* fbMesh = CreateTexturedQuad(roomWidth*2, roomHeight*3, roomWidth * roomTilesPerUnit, roomHeight * roomTilesPerUnit);
    mMeshes.push_back(fbMesh);
    // left and right walls
    Mesh* lrMesh = CreateTexturedQuad(roomDepth, roomHeight*3, roomDepth * roomTilesPerUnit, roomHeight * roomTilesPerUnit);
    mMeshes.push_back(lrMesh);
    // ceiling and floor
    Mesh* cfMesh = CreateTexturedQuad(roomWidth*2, roomDepth, roomWidth * roomTilesPerUnit, roomDepth * roomTilesPerUnit);
    mMeshes.push_back(cfMesh);

    //
    // Load textures
    //

    std::vector<std::string> texNames;
    texNames.push_back("textures/CarvedSandstone.tga");
    texNames.push_back("textures/rocky.tga");
    texNames.push_back("textures/bricks_overpainted_blue_9291383.tga");
    texNames.push_back("textures/water_drops_on_metal_3020602.tga");
    texNames.push_back("textures/grass.tga");
    texNames.push_back("textures/white.tga");
    texNames.push_back("textures/yo.tga");
    texNames.push_back("textures/black.tga");
	texNames.push_back("textures/lava.tga");

    for (unsigned i = 0; i < texNames.size(); i++)
        mTextures.push_back(new Texture(texNames[i], GL_MIRRORED_REPEAT, GL_LINEAR));

    //
    // Create materials
    //

    // add a material for each loaded texture (with default tint)
    for (unsigned i = 0; i < texNames.size(); i++)
        mMaterials.push_back(new Material(mTextures[i]));

    //
    // set extra material properties
    //

    // water drops (sharp and strong specular highlight)
    mMaterials[3]->specular = glm::vec3(1.0f, 1.0f, 1.0f);
    mMaterials[3]->shininess = 128;

    // skin (washed out and faint specular highlight)
    mMaterials[4]->specular = glm::vec3(0.3f, 0.3f, 0.3f);
    mMaterials[4]->shininess = 8;

    // white
    mMaterials[5]->specular = glm::vec3(0.75f, 0.75f, 0.75f);
    mMaterials[5]->shininess = 64;

    // yo
    mMaterials[6]->specular = glm::vec3(1.0f, 0.0f, 1.0f);  // magenta highlights
    mMaterials[6]->shininess = 16;

    // black
    mMaterials[7]->specular = glm::vec3(1.0f, 0.5f, 0.0f);  // orange hightlights
    mMaterials[7]->shininess = 16;

    //
    // Create entities
    //

    unsigned numRows = mMaterials.size();
    float spacing = 3;
    float z = 0.5f * spacing * numRows;
    for (unsigned i = 2; i < mMaterials.size(); i++) {
        // cube
        //mEntities.push_back(new Entity(mMeshes[0], mMaterials[i], Transform(-4.0f, 0.0f, z)));
        // chunky cylinder
       // mEntities.push_back(new Entity(mMeshes[1], mMaterials[i], Transform( 0.0f, 0.0f, z)));
        // smooth cylinder
        //mEntities.push_back(new Entity(mMeshes[2], mMaterials[i], Transform( 4.0f, 0.0f, z)));

        // next row
       // z -= spacing;
    }



	/*
	*
	*
	*
	* MY STUFF
	*
	*
	*/

	//my custom entities 
	Material* myMaterial = new Material(mTextures[5]);
	Mesh* cubeMesh = CreateTexturedCube(5);
	
	Material* texmex = new Material(new Texture("textures/target.tga", GL_REPEAT, GL_LINEAR));
	
	Material* emptyTex = new Material(new Texture());
	//active = new Entity(wireframeCube, myMaterial, Transform(-10.0f, 0.0f, z));
	//mEntities.push_back(active);

	//cube 
	//glm::vec3 min = glm::vec3(-0.5f, -0.5f, -0.5f);
	//glm::vec3 max = glm::vec3(0.5f, 0.5f, 0.5f);
	//active = new Entity(cubeMesh, texmex, Transform(0.0f, 0.0f, 0), min, max);
	//mEntities.push_back(active);



	//small cube 
	/*Mesh* smallCubeMesh = CreateTexturedCube(0.5f);
	smallCube = new Entity(smallCubeMesh, myMaterial, Transform(0.0f, 0.0f, -15.0f));
	mEntities.push_back(smallCube);*/

	//wireframe cube
	{
		/*float width = 5.0f;
		glm::vec3 min = glm::vec3(-0.5f, -0.5f, -0.5f) * width;
		glm::vec3 max = glm::vec3(0.5f, 0.5f, 0.5f) * width;
		Mesh* wireframeMesh = CreateWireframeCube(width);
		wireframeCube = new Entity(wireframeMesh, emptyTex, Transform(0.0f, 0.0f, 0.0f), min, max);
		mEntities.push_back(wireframeCube);*/
	}

	arrow = new Arrow();
	Entity* target = arrow->targetEntity;
	//wireframeCube = target;
	mEntities.push_back(arrow);
	mEntities.push_back(target);



	//LOAD MODELS FOR FUN
	Material* texy = new Material(new Texture("textures/green.tga", GL_REPEAT, GL_LINEAR));

	Material* objTexture = new Material(mTextures[0]);
	Mesh* bokoblin = LoadMesh("meshes/Bokoblin-centered.obj");
	//Entity* bunny = new Entity(bokoblin, texy, Transform(0.0f, 0.0f, -22.0f));
	//mEntities.push_back(bunny);

	

	std::vector<Texture*> texs;
	texs.push_back(new Texture("textures/green.tga", GL_REPEAT, GL_LINEAR));
	texs.push_back(new Texture("textures/red.tga", GL_REPEAT, GL_LINEAR));
	texs.push_back(new Texture("textures/blue.tga", GL_REPEAT, GL_LINEAR));

	// water drops (sharp and strong specular highlight)
	mMaterials[3]->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	mMaterials[3]->shininess = 128;

	// skin (washed out and faint specular highlight)
	mMaterials[4]->specular = glm::vec3(0.3f, 0.3f, 0.3f);
	mMaterials[4]->shininess = 8;

	// white
	mMaterials[5]->specular = glm::vec3(0.75f, 0.75f, 0.75f);
	mMaterials[5]->shininess = 64;

	for (int i = 0; i < 3; i++)
	{
		Material* m = new Material(texs[i]);
		m->specular = glm::vec3(0.3f, 0.3f, 0.3f);
		m->shininess = 8;

		Entity* e = new Entity(bokoblin, m, Transform(10.0f *i -15, 0.0f, 22.0f));
		e->createBoundingBox();
		//e->rotate(180, glm::vec3(0, 1.0f, 0));
		mEntities.push_back(e);

	}




    //
    // Create room
    //

    // back wall
    mEntities.push_back(new Entity(fbMesh, mMaterials[8], Transform(0, 0, -0.5f * roomDepth)));
    //// front wall
    mEntities.push_back(new Entity(fbMesh, mMaterials[8], Transform(0, 0, 0.5f * roomDepth, glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)))));
    //// left wall
    mEntities.push_back(new Entity(lrMesh, mMaterials[8], Transform(-0.5f * roomWidth*2, 0, 0, glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)))));
    //// right wall
    mEntities.push_back(new Entity(lrMesh, mMaterials[8], Transform(0.5f * roomWidth*2, 0, 0, glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)))));
    //// floor
    mEntities.push_back(new Entity(cfMesh, mMaterials[8], Transform(0, -3.5, 0, glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))));
    //// ceiling
    //mEntities.push_back(new Entity(cfMesh, mMaterials[0], Transform(0, 0.5f * roomHeight, 0, glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)))));

    //
    // create the camera
    //

    mCamera = new Camera(this);
    mCamera->setPosition(1, 2, -12);
    mCamera->lookAt(1, 1, 0);
    mCamera->setSpeed(2);

    // create shader program for debug geometry
    mDbgProgram = new ShaderProgram("shaders/vpc-vs.glsl",
                                    "shaders/vcolor-fs.glsl");

    // create geometry for axes
    mAxes = CreateAxes(2);

	spline_t = 0;

    CHECK_GL_ERRORS("initialization");
}

void BasicSceneRenderer::shutdown()
{
    for (unsigned i = 0; i < mPrograms.size(); i++)
        delete mPrograms[i];
    mPrograms.clear();

    delete mDbgProgram;
    mDbgProgram = NULL;

    delete mCamera;
    mCamera = NULL;

    for (unsigned i = 0; i < mEntities.size(); i++)
        delete mEntities[i];
    mEntities.clear();

    for (unsigned i = 0; i < mMeshes.size(); i++)
        delete mMeshes[i];
    mMeshes.clear();

    for (unsigned i = 0; i < mMaterials.size(); i++)
        delete mMaterials[i];
    mMaterials.clear();
    
    for (unsigned i = 0; i < mTextures.size(); i++)
        delete mTextures[i];
    mTextures.clear();

    delete mDbgProgram;
    mDbgProgram = NULL;
    
    delete mAxes;
    mAxes = NULL;
}

void BasicSceneRenderer::resize(int width, int height)
{
    glViewport(0, 0, width, height);

    // compute new projection matrix
    mProjMatrix = glm::perspective(glm::radians(50.f), width / (float)height, 0.1f, 1000.0f);
}

void BasicSceneRenderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // activate current program
    ShaderProgram* prog = mPrograms[mLightingModel];
    prog->activate();

    // send projection matrix
    prog->sendUniform("u_ProjectionMatrix", mProjMatrix);

    // send the texture sampler id to shader
    prog->sendUniformInt("u_TexSampler", 0);

    // get the view matrix from the camera
    glm::mat4 viewMatrix = mCamera->getViewMatrix();

    //
    // light setup depends on lighting model
    //

    if (mLightingModel == PER_VERTEX_DIR_LIGHT) {

        //----------------------------------------------------------------------------------//
        //                                                                                  //
        // Basic directional light (no ambient, specular, or emissive contributions)        //
        //                                                                                  //
        //----------------------------------------------------------------------------------//

        // direction to light
        glm::vec4 lightDir = glm::normalize(glm::vec4(1, 3, 2, 0));

        // send light direction in eye space
        prog->sendUniform("u_LightDir", glm::vec3(viewMatrix * lightDir));

        // send light color/intensity
        prog->sendUniform("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    } else if (mLightingModel == BLINN_PHONG_PER_FRAGMENT_DIR_LIGHT) {

        //----------------------------------------------------------------------------------//
        //                                                                                  //
        // Directional light with ambient, specular, and emissive contributions             //
        //                                                                                  //
        //----------------------------------------------------------------------------------//

        prog->sendUniform("u_AmbientLightColor", glm::vec3(0.2f, 0.2f, 0.2f));

        // direction to light
        glm::vec4 lightDir = glm::normalize(glm::vec4(1, 3, 2, 0));

        // send light direction in eye space
        prog->sendUniform("u_LightDir", glm::vec3(viewMatrix * lightDir));

        // send light color/intensity
        prog->sendUniform("u_LightColor", glm::vec3(0.8f, 0.8f, 0.8f));

    } else if (mLightingModel == BLINN_PHONG_PER_FRAGMENT_POINT_LIGHT) {

        //----------------------------------------------------------------------------------//
        //                                                                                  //
        // Point light with ambient, specular, and emissive contributions, and attenuation  //
        //                                                                                  //
        //----------------------------------------------------------------------------------//

        prog->sendUniform("u_AmbientLightColor", glm::vec3(0.1f, 0.1f, 0.1f));

        // point light position
        glm::vec3 lightPos = glm::vec3(0, 7, 20.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.8f);

        // send light position in eye space
        prog->sendUniform("u_LightPos", glm::vec3(viewMatrix * glm::vec4(lightPos, 1)));

        // send light color/intensity
        prog->sendUniform("u_LightColor", lightColor);

        prog->sendUniform("u_AttQuat", 0.005f);
        prog->sendUniform("u_AttLin", 0.05f);
        prog->sendUniform("u_AttConst", 1.0f);

        // render the light as an emissive cube, if desired
        if (mVisualizePointLights) {
            const Mesh* lightMesh = mMeshes[0];
            lightMesh->activate();
            glBindTexture(GL_TEXTURE_2D, mTextures[7]->id());  // use black texture
            prog->sendUniform("u_MatEmissiveColor", lightColor);
            prog->sendUniform("u_ModelviewMatrix", glm::translate(viewMatrix, glm::vec3(lightPos)));
            prog->sendUniform("u_NormalMatrix", glm::mat3(1.0f));
            lightMesh->draw();
        }

    } else if (mLightingModel == BLINN_PHONG_PER_FRAGMENT_MULTI_LIGHT) {

        //----------------------------------------------------------------------------------//
        //                                                                                  //
        // Multiple directional/point lights                                                //
        //                                                                                  //
        //----------------------------------------------------------------------------------//

        prog->sendUniform("u_AmbientLightColor", glm::vec3(0.1f, 0.1f, 0.1f));

        prog->sendUniformInt("u_NumDirLights", 1);
        prog->sendUniformInt("u_NumPointLights", 4);

        // directional light
        glm::vec4 lightDir = glm::normalize(glm::vec4(1, 3, 2, 0));
        prog->sendUniform("u_DirLights[0].dir", glm::vec3(viewMatrix * lightDir));
        prog->sendUniform("u_DirLights[0].color", glm::vec3(0.3f, 0.3f, 0.3f));

        // point light
        glm::vec3 lightPos1 = glm::vec3(-7, 5, -12);
        glm::vec3 lightColor1 = glm::vec3(1.0f, 1.0f, 1.0f);
        prog->sendUniform("u_PointLights[0].pos", glm::vec3(viewMatrix * glm::vec4(lightPos1, 1)));
        prog->sendUniform("u_PointLights[0].color", lightColor1);
        prog->sendUniform("u_PointLights[0].attQuat", 0.01f);
        prog->sendUniform("u_PointLights[0].attLin", 0.1f);
        prog->sendUniform("u_PointLights[0].attConst", 1.0f);

        // point light
        glm::vec3 lightPos2 = glm::vec3(7, 5, -12);
        glm::vec3 lightColor2 = glm::vec3(1.0f, 1.0f, 1.0f);
        prog->sendUniform("u_PointLights[1].pos", glm::vec3(viewMatrix * glm::vec4(lightPos2, 1)));
        prog->sendUniform("u_PointLights[1].color", lightColor2);
        prog->sendUniform("u_PointLights[1].attQuat", 0.01f);
        prog->sendUniform("u_PointLights[1].attLin", 0.1f);
        prog->sendUniform("u_PointLights[1].attConst", 1.0f);

        // point light
        glm::vec3 lightPos3 = glm::vec3(-7, -5, 15);
        glm::vec3 lightColor3 = glm::vec3(1.0f, 1.0f, 1.0f);
        prog->sendUniform("u_PointLights[2].pos", glm::vec3(viewMatrix * glm::vec4(lightPos3, 1)));
        prog->sendUniform("u_PointLights[2].color", lightColor3);
        prog->sendUniform("u_PointLights[2].attQuat", 0.01f);
        prog->sendUniform("u_PointLights[2].attLin", 0.1f);
        prog->sendUniform("u_PointLights[2].attConst", 1.0f);

		// point light
		glm::vec3 lightPos4 = glm::vec3(0, 10, 22);
		glm::vec3 lightColor4 = glm::vec3(1.0f, 1.0f, 1.0f);
		prog->sendUniform("u_PointLights[3].pos", glm::vec3(viewMatrix * glm::vec4(lightPos4, 1)));
		prog->sendUniform("u_PointLights[3].color", lightColor4);
		prog->sendUniform("u_PointLights[3].attQuat", 0.01f);
		prog->sendUniform("u_PointLights[3].attLin", 0.1f);
		prog->sendUniform("u_PointLights[3].attConst", 1.0f);

        // render the point lights as emissive cubes, if desirable
        if (mVisualizePointLights) {
            glBindTexture(GL_TEXTURE_2D, mTextures[7]->id());  // use black texture
            prog->sendUniform("u_NormalMatrix", glm::mat3(1.0f));
            const Mesh* lightMesh = mMeshes[0];
            lightMesh->activate();
            prog->sendUniform("u_MatEmissiveColor", lightColor1);
            prog->sendUniform("u_ModelviewMatrix", glm::translate(viewMatrix, glm::vec3(lightPos1)));
            lightMesh->draw();
            prog->sendUniform("u_MatEmissiveColor", lightColor2);
            prog->sendUniform("u_ModelviewMatrix", glm::translate(viewMatrix, glm::vec3(lightPos2)));
            lightMesh->draw();
            prog->sendUniform("u_MatEmissiveColor", lightColor3);
            prog->sendUniform("u_ModelviewMatrix", glm::translate(viewMatrix, glm::vec3(lightPos3)));
            lightMesh->draw();
			prog->sendUniform("u_MatEmissiveColor", lightColor4);
			prog->sendUniform("u_ModelviewMatrix", glm::translate(viewMatrix, glm::vec3(lightPos4)));
			lightMesh->draw();
        }
    }

    // render all entities
    for (unsigned i = 0; i < mEntities.size(); i++) {

        Entity* ent = mEntities[i];
		const Texture* tex = ent->mMaterial->tex;
	
		//only for bounding boxes
		//render without textures/lighting
		if (ent->hasBoundingBox == true)
		{
			mDbgProgram->activate();
			mDbgProgram->sendUniform("u_ModelviewMatrix", viewMatrix * ent->getWorldMatrix());
			ent->boundingBox->active->activate();
			ent->boundingBox->active->draw();
		}
		
		
		prog->activate();
			
		// use the entity's material
		const Material* mat = ent->getMaterial();
		glBindTexture(GL_TEXTURE_2D, mat->tex->id());   // bind texture
		prog->sendUniform("u_Tint", mat->tint);     // send tint color

		// send the Blinn-Phong parameters, if required
		if (mLightingModel > PER_VERTEX_DIR_LIGHT) {
			prog->sendUniform("u_MatEmissiveColor", mat->emissive);
			prog->sendUniform("u_MatSpecularColor", mat->specular);
			prog->sendUniform("u_MatShininess", mat->shininess);
		}

		// compute modelview matrix
		glm::mat4 modelview = viewMatrix * ent->getWorldMatrix();

		// send the entity's modelview and normal matrix
		prog->sendUniform("u_ModelviewMatrix", modelview);
		prog->sendUniform("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(modelview))));

		// use the entity's mesh
		const Mesh* mesh = ent->getMesh();
		mesh->activate();
		mesh->draw();
		
    }

	//draw stuff without materials/textures or using simple colorshaders here

	// load shader with no lighting
    mDbgProgram->activate();
    mDbgProgram->sendUniform("u_ProjectionMatrix", mProjMatrix);

	//DRAW 3 AXIS ON ACTIVE OBJECT
    /*Entity* activeEntity = mEntities[mActiveEntityIndex];
    mDbgProgram->sendUniform("u_ModelviewMatrix", viewMatrix * activeEntity->getWorldMatrix());
    mAxes->activate();
    mAxes->draw();*/

	//DRAW 3 AXIS AT ORIGIN
	mDbgProgram->sendUniform("u_ModelviewMatrix", viewMatrix * glm::translate(glm::mat4(), glm::vec3(0,10,0)));
	mAxes->activate();
	mAxes->draw();

	//DRAW RAY
	/*mDbgProgram->sendUniform("u_ModelviewMatrix", viewMatrix * ray->getWorldMatrix());
	rayMesh->activate();
	rayMesh->draw();*/

	//wireframe cube
	//mDbgProgram->sendUniform("u_ModelviewMatrix", viewMatrix * wireframeCube->getWorldMatrix());
	//wireframeCube->getMesh()->activate();
	//wireframeCube->getMesh()->draw();

	mDbgProgram->sendUniform("u_ModelviewMatrix", viewMatrix * arrow->getWorldMatrix());
	arrow->directionRay->activate();
	arrow->directionRay->draw();

	//draw bounding box for 

    CHECK_GL_ERRORS("drawing");
}

bool BasicSceneRenderer::update(float dt)
{
	//SHOOTING

	int toDelete = -1;
	//for each, if has bounding box, 
	for (int i = 0; i < mEntities.size(); i++)
	{
		Entity* e = mEntities[i];

		if (e->hasBoundingBox == true)
		{
			//MOVE THEM TOWARD ME
			//e->translate(glm::vec3(0, 0, -0.01));
			glm::vec3 dir = glm::vec3(0.0f, 0.0f, -10.0f) - e->getPosition();
			e->translate(glm::normalize(dir) * 0.1f);

			//check for collision with ray
			bool result = arrow->isIntersecting(e);
			if (result == true)
			{
				//toDelete = i;
				e->boundingBox->active = e->boundingBox->mMesh2;

				//if arrow touching a box
				if (arrow->getPosition().z > e->getMin().z)
				{
					//toDelete = i;

					//move guy
					float min = -10;
					float max = 10;
					float num = (min + (rand() % (int)(max - min + 1)));
					e->setPosition(glm::vec3(num, 0.0f, 22.0f));

					//reset arrow, not good yet
					//arrow->isMoving = false;
					//arrow->setPosition(glm::vec3(0, 0, -10.0f));
				}
			}
			else
			{
				e->boundingBox->active = e->boundingBox->mMesh;
			}
		}

	}

	if (toDelete != -1) mEntities.erase(mEntities.begin() + toDelete);




	




    const Keyboard* kb = getKeyboard();

	arrow->update(dt);

	if (kb->keyPressed(KC_SPACE))
	{
		arrow->isMoving = true;
	
	}




    if (kb->keyPressed(KC_ESCAPE))
        return false;

    // move forward through our list of entities
    if (kb->keyPressed(KC_X)) {
        // compute next entity index
        ++mActiveEntityIndex;
        if (mActiveEntityIndex >= (int)mEntities.size())
            mActiveEntityIndex = 0;
    }

    // move backward through our list of entities
    if (kb->keyPressed(KC_Z)) {
        // compute previous entity index
        --mActiveEntityIndex;
        if (mActiveEntityIndex < 0)
            mActiveEntityIndex = (int)mEntities.size() - 1;
    }

    // get the entity to manipulate
    Entity* activeEntity = mEntities[mActiveEntityIndex];

    // rotate the entity
    float rotSpeed = 35;
    float rotAmount = rotSpeed * dt;
    if (kb->isKeyDown(KC_LEFT))
        activeEntity->rotate(rotAmount, 0, 1, 0);
    if (kb->isKeyDown(KC_RIGHT))
        activeEntity->rotate(-rotAmount, 0, 1, 0);
    if (kb->isKeyDown(KC_UP))
		activeEntity->rotate(-rotAmount, 1, 0, 0);
    if (kb->isKeyDown(KC_DOWN))
		activeEntity->rotate(rotAmount, 1, 0, 0);
        

    // reset entity orientation
    if (kb->keyPressed(KC_R))
        activeEntity->setOrientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    float speed = 3;
    float disp = speed * dt;

    // move entity along world axes
    if (kb->isKeyDown(KC_I))
        activeEntity->translate(0, 0, disp);
    if (kb->isKeyDown(KC_K))
        activeEntity->translate(0, 0, -disp); //holding shift will double the translation
    if (kb->isKeyDown(KC_L))
        activeEntity->translate(disp, 0, 0);
    if (kb->isKeyDown(KC_J))
        activeEntity->translate(-disp, 0, 0);

    // move entity along entity's local axes
    if (kb->isKeyDown(KC_T))
        activeEntity->translateLocal(0, 0, disp);
    if (kb->isKeyDown(KC_G))
        activeEntity->translateLocal(0, 0, -disp);
    if (kb->isKeyDown(KC_F))
        activeEntity->translateLocal(disp, 0, 0);
    if (kb->isKeyDown(KC_H))
        activeEntity->translateLocal(-disp, 0, 0);

    // change lighting models
    if (kb->keyPressed(KC_1))
        mLightingModel = PER_VERTEX_DIR_LIGHT;
    if (kb->keyPressed(KC_2))
        mLightingModel = BLINN_PHONG_PER_FRAGMENT_DIR_LIGHT;
    if (kb->keyPressed(KC_3))
        mLightingModel = BLINN_PHONG_PER_FRAGMENT_POINT_LIGHT;
    if (kb->keyPressed(KC_4))
        mLightingModel = BLINN_PHONG_PER_FRAGMENT_MULTI_LIGHT;

    // toggle visualization of point lights
    if (kb->keyPressed(KC_TAB))
        mVisualizePointLights = !mVisualizePointLights;

    // update the camera
    mCamera->update(dt);

	spline_t += 1; //not correct

	//
	//glm::vec3 point = ray->getMin();
	//glm::vec3 dir = glm::normalize(ray->getMax() - ray->getMin());
	//glm::vec3 result;
	//float tmin;

	////smallCube->setPosition(result);

	////int val = IntersectRayAABB(point, dir, active, tmin, result);
	//
	//bool val = intersect(active, point, dir);
	//std::cout << val << std::endl;

	//int z = 2;



	//origin is min/max




	return true;
}


void Swap(float& t1, float& t2)
{
	float temp = t1;
	t1 = t2;
	t2 = temp;
	int i = 2;
}


// Intersect ray R(t) = p + t*d against AABB a. When intersecting,
// return intersection distance tmin and point q of intersection
int BasicSceneRenderer::IntersectRayAABB(glm::vec3 p, glm::vec3 d, Entity* a, float &tmin, glm::vec3 &q)
{
	tmin = 0; // set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX; // set to max distance ray can travel (for segment)
						  // For all three slabs
	for (int i = 0; i < 3; i++) {
		if (std::abs(d[i]) < std::numeric_limits<float>::epsilon()) {
			// Ray is parallel to slab. No hit if origin not within slab
			if (p[i] < a->getMin()[i] || p[i] > a->getMax()[i]) return 0;
		}
		else {
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / d[i];
			float t1 = (a->getMin()[i] - p[i]) * ood;
			float t2 = (a->getMax()[i] - p[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) Swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) {
				return 0;
			}
		}
	}
	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	q = p + d * tmin;
	return 1;
}


bool BasicSceneRenderer::intersect(Entity* ent, glm::vec3 org, glm::vec3 dir) {
	glm::vec3 lb = ent->getMin();  //left bottom of box
	glm::vec3 rt = ent->getMax();  //right top of box

	glm::vec3 dirfrac;
	float t;

	// r.dir is unit direction vector of ray
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (lb.x - org.x)*dirfrac.x;
	float t2 = (rt.x - org.x)*dirfrac.x;
	float t3 = (lb.y - org.y)*dirfrac.y;
	float t4 = (rt.y - org.y)*dirfrac.y;
	float t5 = (lb.z - org.z)*dirfrac.z;
	float t6 = (rt.z - org.z)*dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0)
	{
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		t = tmax;
		return false;
	}

	t = tmin;
	return true;
}
