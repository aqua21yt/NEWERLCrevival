#include <common.h>
#include <game.h>
#include <stage.h>
#include <g3dhax.h>
#include <sfx.h>
#include <gamescene.h> //this doesn't contains only the TimeKeeper class, but also much shit. I should maybe have named it "junk.h"
#include "boss.h"

int doWait = 0; //global doWait variable to limit the spikeball throwing


class daSpikyHax_c : public dEn_c { //the class, to declare the things we'll use
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c bodyModel;

	int alreadyOnTop = 0;

	float XSpeed;
	u32 cmgr_returnValue;
	bool isOnTopOfTile;

	static daSpikyHax_c *build();

	void updateModelMatrices();
	bool calculateTileCollisions();

	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daSpikyHax_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Die);
};

daSpikyHax_c *daSpikyHax_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daSpikyHax_c));
	return new(buffer) daSpikyHax_c;
}

///////////////////////
// States
///////////////////////


	CREATE_STATE(daSpikyHax_c, Walk);
	CREATE_STATE(daSpikyHax_c, Die);


////////////////////////
// Collision Functions
////////////////////////


	void daSpikyHax_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
		u16 name = ((dEn_c*)apOther->owner)->name;                                                                           //get the name of the actor the spikeball collided with

		if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE //list of actors ignored by the spikeball
			|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND
			|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
			|| name == EN_JUMPDAI || name == EN_ITEM || name == EN_STAR_COIN)
			{ return; }


		dEn_c::spriteCollision(apThis, apOther);                                                                             //do as if it hit a sprite
		((dEn_c*)apOther->owner)->collisionCat13_Hammer(apThis, apOther);                                                    //do as if it was touched by a hammer
	}

	void daSpikyHax_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

	}

	void daSpikyHax_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

	}

	bool daSpikyHax_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	}

	bool daSpikyHax_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}

	bool daSpikyHax_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}

	bool daSpikyHax_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
		doStateChange(&StateID_Die);
		return true;
	}

	bool daSpikyHax_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
		return false;
	}

	bool daSpikyHax_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}

	bool daSpikyHax_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
		return false;
	}

	bool daSpikyHax_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
		return false;
	}

	bool daSpikyHax_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
		return false;
	}
	
	bool daSpikyHax_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
		return false;
	}


bool daSpikyHax_c::calculateTileCollisions() {

	/****************************/
	/***Tile collision detector**/
	/*Returns true if hit a wall*/
	/****************************/


	HandleXSpeed();                                      //consider the X speed
	HandleYSpeed();                                      //consider the Y speed
	doSpriteMovement();                                  //execute the speed movements

	cmgr_returnValue = collMgr.isOnTopOfTile();          //check if the sprite is on top of a tile
	collMgr.calculateBelowCollisionWithSmokeEffect();    //duh

	stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);      //literally stuff related to collisions

	float xDelta = pos.x - last_pos.x;                   //just read dude

	if (xDelta >= 0.0f) {                                //change of direction if needed
		direction = 0;
	}
	else {
		direction = 1;
	}

	if (collMgr.isOnTopOfTile()) {                       //if the sprite is on top of a tile
		if(alreadyOnTop == 0) {                          //if it just landed
			PlaySound(this, SE_OBJ_ROCK_LAND);           //play SFX
			alreadyOnTop = 1;                            //now it's already on the to^p
		}

		if (cmgr_returnValue == 0)                       //just read ?
			isOnTopOfTile = true;

		speed.y = 0.0f;                                  //no Y speed anymore cuz it's on the ground
		max_speed.x = (direction == 1) ? -1.5f : 1.5f;   //maximum X speed re-setting
		this->x_speed_inc = 0.0f;                        //no X speed incrementation
		this->speed.x = (direction == 1) ? -1.5f : 1.5f; //X speed re-setting
	}
	else {                                               //if the sprite in in mid-air
		alreadyOnTop = 0;                                //it's no loner on the top of a tile
	}

	collMgr.calculateAdjacentCollision(0);               //literally calculate the adjacent collision

	if (collMgr.outputMaybe & (0x15 << direction)) {     //if hit a wall
		if (collMgr.isOnTopOfTile()) {                   //if on the top of a tile
			isOnTopOfTile = true;
		}
		return true;                                     //returns true duh
	}
	return false;                                        //if didn't hit a wall
}

int daSpikyHax_c::onCreate() {

	this->deleteForever = true;                                         //FOREVAAAAAAAAH

	/****************/
	/*Model Creation*/
	/****************/

	allocator.link(-1, GameHeaps[0], 0, 0x20);                          //allocator linking

	this->resFile.data = getResource("hammerM", "g3d/iron_ball.brres"); //get the brres in the arc file
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("iron_ball");       //get the mdl0
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);                      //setup it
	SetupTextures_MapObj(&bodyModel, 0);                                //setup shaders

	allocator.unlink();                                                 //allocator unlinking

	/****************/
	/*Values Setting*/
	/****************/

	this->scale = (Vec){0.7, 0.7, 0.7};                                 //scale setting

	this->rot.x = 0;                                                    //X rotation setting
	this->rot.y = 0;                                                    //Y rotation setting
	this->rot.z = 0;                                                    //Z rotation setting
	this->direction = this->settings;                                   //heading where the player look.
	
	this->speed.x = 0.0;                                                //X speed setting
	this->speed.y = 3.0;                                                //Y speed setting
	this->max_speed.x = 0.0;                                            //X maximum speed setting
	this->x_speed_inc = 0.0;                                            //X speed incrementation setting
	this->XSpeed = 3.0;                                                 //XSpeed value (used later) setting

	/********************/
	/*Collision Creating*/
	/********************/

	ActivePhysics::Info HitMeBaby;                                      //ActivePhysics::Info creating

	HitMeBaby.xDistToCenter = 0.0;                                      //no X distance to center duh
	HitMeBaby.yDistToCenter = 0.0;                                      //no Y distance to center duh

	HitMeBaby.xDistToEdge = 7.5;                                        //1 block wide
	HitMeBaby.yDistToEdge = 7.5;                                        //1 block high		

	HitMeBaby.category1 = 0x3;                                          //idk what is this, ask treeki
	HitMeBaby.category2 = 0x0;                                          //^
	HitMeBaby.bitfield1 = 0x6F;                                         //^
	HitMeBaby.bitfield2 = 0xffbafffe;                                   //^
	HitMeBaby.unkShort1C = 0;                                           //^
	HitMeBaby.callback = &dEn_c::collisionCallback;                     //collision callback

	this->aPhysics.initWithStruct(this, &HitMeBaby);                    //initating the collision
	this->aPhysics.addToList();                                         //add to the collision list

	/********************/
	/*Colliders Creating*/
	/********************/

	// These fucking rects do something for the tile rect
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	static const lineSensor_s below(-5<<12, 5<<12, -9<<12);            //Below collider:    unk, unk, y pos 
	static const pointSensor_s above(0<<12, 12<<12);                   //Above collider:    unk, unk, unk
	static const lineSensor_s adjacent(3<<12, 6<<12, 6<<12);           //Adjacent collider: unk, unk, unk

	collMgr.init(this, &below, &above, &adjacent);                     //initating colliders
	collMgr.calculateBelowCollisionWithSmokeEffect();                  //dude, the description of this line IS in the line, so what are you reading ?

	cmgr_returnValue = collMgr.isOnTopOfTile();                        //get if the sprite is in top of a tile (GOD DAMMIT IT IS WROTE)

	if (collMgr.isOnTopOfTile())                                       //just... read !
		isOnTopOfTile = false;
	else
		isOnTopOfTile = true;

	this->pos.y += 16;                                                 //because the game


	doStateChange(&StateID_Walk);                                      //switch to the walk state (even if it's not really walking but shut up)

	this->onExecute();
	return true;
}

int daSpikyHax_c::onDelete() {
	return true;
}

int daSpikyHax_c::onExecute() {
	acState.execute();                                   //execute the states
	updateModelMatrices();                               //updates the model

	bodyModel._vf1C();                                   //Animations, iirc

	this->rot.z += (this->settings == 1) ? 2000 : -2000; //Rotating it depending of its spawning direction
	
	PlaySound(this, SE_PLY_WALK_METAL);                  //Play SFX

	return true;
}

int daSpikyHax_c::onDraw() {
	bodyModel.scheduleForDrawing();                      //shedule the model drawing

	return true;
}

void daSpikyHax_c::updateModelMatrices() {               //updates the model:
	matrix.translation(pos.x, pos.y - 2.0, pos.z);       //its position
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);     //its rotation

	bodyModel.setDrawMatrix(matrix);                     //its draw matrix
	bodyModel.setScale(&scale);                          //its scale
	bodyModel.calcWorld(false);                          //what's that duh
}


///////////////
// Walk State
///////////////
	void daSpikyHax_c::beginState_Walk() {
		this->max_speed.x = (this->direction) ? -this->XSpeed : this->XSpeed; //set the maximum X speed depending of the spawning direction
		this->speed.x = (this->direction) ? -3.0 : 3.0;                       //set the current X speed depending of the spawning direction
		this->x_speed_inc = (this->direction) ? 0.6 : -0.6;                   //set the X speed increment depending of the spawning direction

		this->max_speed.y = -4;                                               //set the maximum Y speed
		this->speed.y = 6.0;                                                  //set the current Y speed
		this->y_speed_inc = -0.5;                                             //set the Y speed increment
	}
	void daSpikyHax_c::executeState_Walk() {
		bool ret = calculateTileCollisions();                                 //calculate the tiles collision
		if (ret) {
			doStateChange(&StateID_Die);                                      //if hit a wall, die
		}
	}
	void daSpikyHax_c::endState_Walk() { }


///////////////
// Die State
///////////////
	void daSpikyHax_c::beginState_Die() {
		this->removeMyActivePhysics();           //Remove the collision
	}
	void daSpikyHax_c::executeState_Die() { 
		PlaySound(this, SE_OBJ_WOOD_BOX_BREAK);  //Play SFX
		Vec2 nyeh = {this->pos.x, this->pos.y};  //Get the current position as a Vec2
		this->spawnHitEffectAtPosition(nyeh);    //Spawn Hit Effect at this position (duh)
		doWait = 0;                              //doWait is now 0
		this->Delete(this->deleteForever);       //DELETE FOREVAAAAAAAH
	}
	void daSpikyHax_c::endState_Die() { }



/***********************************************************************************/
/**ALL THE STUFF BELOW IS TESTS, DEBUG MODE AND MOSTLY UNRELATED TO THE SPIKE SUIT**/
/*************BUT I HAD TO PUT IT HERE CUZ LAZYNESS AND GLOBAL VARIABLES************/
/***********************************************************************************/


/***************************/
/**Nybble getting function**/
/***************************/

extern "C" float pow(float num, float power);

int getNybbleValuee(u32 settings, int fromNybble, int toNybble, bool doOSReport) {
	int numberOfNybble = (toNybble  - fromNybble) + 1;               //gets how many nybbles are used for the process (example: nybbles 4-6 -> there's nybbles 4, 5 and 6 used -> numberOfNybble = 3) 
	int valueToUse = 48 - (4 * toNybble);                            //gets the value to use with the bitshift at the end 
	int fShit = pow(16, numberOfNybble) - 1;                         //gets the value to use with the "&" operator at the end 
	if(doOSReport) {
		OSReport("-> (settings >> %d) & 0x%x) => ", valueToUse, fShit);  //debugging
	}
	return ((settings >> valueToUse) & fShit);                       //uses everything to make the nybble value 
}

/******************************************************/
/**The daGabonRock_c class so we can make edits to it**/
/******************************************************/

class daGabonRock_c : dEn_c {
	public:
		int onCreate_orig();
		int getsettings();
};

/******************************************************************/
/**OSReporting the gabon's rock settings for spike suit debugging**/
/******************************************************************/

int daGabonRock_c::getsettings() {
	int orig_val = this->onCreate_orig();
	OSReport("--------\nSpawning daGabonRock_c:\n");
	OSReport("this->settings: nybble 5  -> %d\n", getNybbleValuee(this->settings, 5, 5, true));
	OSReport("this->settings: nybble 6  -> %d\n", getNybbleValuee(this->settings, 6, 6, true));
	OSReport("this->settings: nybble 7  -> %d\n", getNybbleValuee(this->settings, 7, 7, true));
	OSReport("this->settings: nybble 8  -> %d\n", getNybbleValuee(this->settings, 8, 8, true));
	OSReport("this->settings: nybble 9  -> %d\n", getNybbleValuee(this->settings, 9, 9, true));
	OSReport("this->settings: nybble 10 -> %d\n", getNybbleValuee(this->settings, 10, 10, true));
	OSReport("this->settings: nybble 11 -> %d\n", getNybbleValuee(this->settings, 11, 11, true));
	OSReport("this->settings: nybble 12 -> %d\n", getNybbleValuee(this->settings, 12, 12, true));
	OSReport("this->pos.x: %d\n", this->pos.x);
	OSReport("this->pos.y: %d\n", this->pos.y);
	OSReport("this->pos.z: %d\n", this->pos.z);
	OSReport("this->scale.x: %d\n", this->scale.x);
	OSReport("this->scale.y: %d\n", this->scale.y);
	OSReport("this->scale.z: %d\n", this->scale.z);
	OSReport("this->direction: %d\n", this->direction);
	if(getNybbleValuee(this->settings, 12, 12, false) > 1) {
		int playerID = getNybbleValuee(this->settings, 6, 6, false);
		dAcPy_c *player = dAcPy_c::findByID(playerID);
		OSReport("player ID: %d\n", playerID);
		OSReport("player direction: %d\n", player->direction); //1 -> facing left | 0 -> facing right
		PlaySound(player, SE_EMY_GABON_ROCK_THROW);
		CreateActor(555, player->direction, player->pos, 0, 0);
		doWait = 60;
	}
	OSReport("--------\n");
	return orig_val;
}