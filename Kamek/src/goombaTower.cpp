#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>

const char* GoombaTowerFileList [] = { "kuribo", NULL };

class daEnGoombaTower_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daEnGoombaTower_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	//bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	

	u8 facing;
	dStageActor_c *goomba;
	dEn_c *belowActor;
	u16 name;
	int timer;
	int occupedValue;
	bool isOnGoomba;

	lineSensor_s belowSensor;
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;
	
	USING_STATES(daEnGoombaTower_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	DECLARE_STATE(Die);
};

CREATE_STATE(daEnGoombaTower_c, Walk);
CREATE_STATE(daEnGoombaTower_c, Turn);
CREATE_STATE(daEnGoombaTower_c, Die);

void daEnGoombaTower_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void daEnGoombaTower_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daEnGoombaTower_c* daEnGoombaTower_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnGoombaTower_c));
	return new(buffer) daEnGoombaTower_c;
}

void daEnGoombaTower_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	name = ((dEn_c*)apOther->owner)->name;
	belowActor = (dEn_c*)apOther->owner;
	if (name == EN_KURIBO || name == AC_LIFT_BOSS_REMOCON_SEESAW){
		isOnGoomba = true;
		goomba->pos.x = belowActor->pos.x;
		goomba->pos.y = belowActor->pos.y + 12.0;
	}
}

void daEnGoombaTower_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	
}

void daEnGoombaTower_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

int daEnGoombaTower_c::onCreate() {
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("kuribo", "g3d/kuribo.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kuribo");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	
	this->timer = 0;
	isOnGoomba = false;

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.2);
	
	allocator.unlink(); 
	
	doStateChange(&StateID_Walk);

	this->onExecute();

	return true;
}

int daEnGoombaTower_c::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone() || isOnGoomba) {
		this->animationChr.setCurrentFrame(0.0);
	}

	acState.execute();

	this->timer++;
	return true;
}

int daEnGoombaTower_c::onDelete() {
	return true;
}

int daEnGoombaTower_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daEnGoombaTower_c::beginState_Walk() {
	this->timer = 0;
	facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	if (facing != this->direction) {
		this->direction = facing;
		this->rot.y = (direction) ? 0xD800 : 0x2800;
	}

	if (!(collMgr.calculateBelowCollision() & (~0x400000))) {
		this->max_speed.x = 0.0;
		this->speed.x = 0.0;
		this->x_speed_inc = 0.0;
		
		belowSensor.flags = SENSOR_LINE;

		belowSensor.lineA = -12 << 12;
		belowSensor.lineB = 12 << 12;
		belowSensor.distanceFromCenter = 0;

		collMgr.init(this, &belowSensor, 0, 0);
		
		speed.y = 0.0f;
		y_speed_inc = -0.1875f;
		max_speed.y = -1.0f;
	}
}
void daEnGoombaTower_c::executeState_Walk() {	
	
	if (!(collMgr.calculateBelowCollision() & (~0x400000))) {
		HandleYSpeed();
		doSpriteMovement();
		UpdateObjectPosBasedOnSpeedValuesReal();
	}
	
	this->pos.x += (direction) ? -0.4 : 0.4;	
	
	this->timer++;	
}
void daEnGoombaTower_c::endState_Walk() {}

void daEnGoombaTower_c::beginState_Turn() {}
void daEnGoombaTower_c::executeState_Turn() {}
void daEnGoombaTower_c::endState_Turn() {}

void daEnGoombaTower_c::beginState_Die() {}
void daEnGoombaTower_c::executeState_Die() {}
void daEnGoombaTower_c::endState_Die() {}