#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>


const char* FirebarsFileContents [] = { "center_firebar", NULL };

class daGreaterFirebar_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	
	int timer;
	int fireBars;
	int fireBalls;
	bool isMovReversed;
	char movSpeed;
	int spinTimer;
	enum spawnBallPhysics {
		FIREBALL_0 = 0,
		FIREBALL_1 = 1,
		FIREBALL_2 = 2,
		FIREBALL_3 = 3,
		FIREBALL_4 = 4,
		FIREBALL_5 = 5,
		FIREBALL_6 = 6,
		FIREBALL_7 = 7,
		FIREBALL_8 = 8,
		FIREBALL_9 = 9,
		FIREBALL_10 = 10,
		FIREBALL_11 = 11,
		FIREBALL_12 = 13,
		FIREBALL_13 = 14,
	};
	
	spawnBallPhysics ballSpawner;
	
	static daGreaterFirebar_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daGreaterFirebar_c);
	DECLARE_STATE(Spin);
};

CREATE_STATE(daGreaterFirebar_c, Spin);

void daGreaterFirebar_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

daGreaterFirebar_c* daGreaterFirebar_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daGreaterFirebar_c));
	return new(buffer) daGreaterFirebar_c;
}

void daGreaterFirebar_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	OSReport("Dude, what shall I write in here? It isn't necessary, but for the game it does?");
	this->timer--;
}

void daGreaterFirebar_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void daGreaterFirebar_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->yoshiCollision(apThis, apOther);
}

bool daGreaterFirebar_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daGreaterFirebar_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

int daGreaterFirebar_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	fireBars = settings >> 12 & 0xF; //nybble 9
	fireBalls = settings >> 12 & 0xF; // nybble 10
	isMovReversed = settings >> 4 & 0xF; //nybble 11
	movSpeed = settings & 0xF; //nybble 12
	
	this->timer = 0;
	this->spinTimer = 0;
	
	this->rot.z + 0x0000;
	
	resFile.data = getResource("center_firebar", "g3d/center_firebar.brres");
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("center_firebar");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info HitMeEvilBar; 
	HitMeEvilBar.xDistToCenter = 0.0; 
	HitMeEvilBar.yDistToCenter = 0.0; 
	HitMeEvilBar.xDistToEdge = 0.0; 
	HitMeEvilBar.yDistToEdge = 0.0; 
	HitMeEvilBar.category1 = 0x3; 
	HitMeEvilBar.category2 = 0x0; 
	HitMeEvilBar.bitfield1 = 0x4F; 
	HitMeEvilBar.bitfield2 = 0xFFFFFFFF; 
	HitMeEvilBar.unkShort1C = 0; 
	HitMeEvilBar.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeEvilBar); 
	this->aPhysics.addToList();


	doStateChange(&StateID_Spin);
	
	this->onExecute();

	return true;
}

int daGreaterFirebar_c::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();
	PlaySound(this, SE_EMY_FIRE_SNAKE_BIGGER);
	
	acState.execute();
	
	return true;
}

int daGreaterFirebar_c::onDelete() {
	return true;
}

int daGreaterFirebar_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daGreaterFirebar_c::beginState_Spin() {
	if (this->timer < (fireBars + 3) * (fireBalls + 3)) {
		if (this->timer % (fireBalls + 3) == 0) {
			this->rot.z += 0xFFFF / (fireBars + 3);
		} else {
			ActivePhysics::Info ballSpawner; 
			ballSpawner.xDistToCenter = 0.0; 
			ballSpawner.yDistToCenter = 12.0 * (this->timer % (fireBalls + 3)); 
			ballSpawner.xDistToEdge = 8.0; 
			ballSpawner.yDistToEdge = 12.0 * (this->timer % (fireBalls + 3)); 
			ballSpawner.category1 = 0x3; 
			ballSpawner.category2 = 0x0; 
			ballSpawner.bitfield1 = 0x4F; 
			ballSpawner.bitfield2 = 0xFFFFFFFF; 
			ballSpawner.unkShort1C = 0; 
			ballSpawner.callback = &dEn_c::collisionCallback; 
			this->aPhysics.initWithStruct(this, &ballSpawner); 
			this->aPhysics.addToList();
			/*
			Vec efPos = {pos.x - 0.0f, pos.y + 12.0f * (this->timer[.0f] % (fireBalls[.0f] + 3.0f)), pos.z + 5000.0f};
			S16Vec nullRot = {0,0,0};
			Vec efScale = {1.0f, 1.0f, 1.0f};
			SpawnEffect("Wm_en_firebar_fire", 0, &efPos, &nullRot, &efScale);
			*/
		}
	}
	
	if (this->timer == (fireBars + 3) * (fireBalls + 3) + 1){
		this->timer = 0; //Dude the most recent sprites don't crash but just don't work properly. S***
	}
	
	ballSpawner++;
	this->timer++;
}
void daGreaterFirebar_c::executeState_Spin() {
	if (isMovReversed) {this->rot.z -= 0x200 * (movSpeed + 1);}
	else {this->rot.z += 0x200 * (movSpeed + 1);}

	if (this->timer >= 1000000000) {
		OSReport("Break down your PC, just kidding");
		this->timer = 0;
		ballSpawner = FIREBALL_1;
	}
	
	this->spinTimer++;
}
void daGreaterFirebar_c::endState_Spin() {}