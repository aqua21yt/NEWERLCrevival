#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <stage.h>


const char* ArrowArcNameList [] = {
	"kanban_yajirushi",
	"kanban_yajirushi_message",
	NULL
};

class daKanbanYajirushi : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	char color;
	int frame;
	bool affectedByGravity;
	bool isMessageArrow;
	int efTimer;
	
	static daKanbanYajirushi* build();

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

	lineSensor_s belowSensor;
	
	USING_STATES(daKanbanYajirushi);
	DECLARE_STATE(Fall);

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

};

CREATE_STATE(daKanbanYajirushi, Fall);

void daKanbanYajirushi::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daKanbanYajirushi::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daKanbanYajirushi* daKanbanYajirushi::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daKanbanYajirushi));
	return new(buffer) daKanbanYajirushi;
}

void daKanbanYajirushi::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	OSReport("don't do anything! You are such as a deco! Unless if...");
	//if (isMessageArrow) {}
}

void daKanbanYajirushi::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void daKanbanYajirushi::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

int daKanbanYajirushi::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	color = settings >> 24 & 0xF; //nybble 6
	affectedByGravity = settings >> 16 & 0xF; //nybble 8
	isMessageArrow = settings >> 12 & 0xF; //nybble 9
	efTimer = 0;
	/*
	char* resName;
	sprintf(resName, "g3d/t%02d.brres", color);
	resName[strlen(resName)] = 0;
	*/
	//if (isMessageArrow) {resFile.data = getResource("kanban_yajirushi_message", "t00.brres");}
	//else {
	resFile.data = getResource("kanban_yajirushi", "t00.brres");
	//}
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kanban_yajirushi");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);
	
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("kanban_yajirushi");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info ArrowPhysc; 
	ArrowPhysc.xDistToCenter = 0.0; 
	ArrowPhysc.yDistToCenter = 0.0; 
	ArrowPhysc.xDistToEdge = 16.0; 
	ArrowPhysc.yDistToEdge = 24.0; 
	ArrowPhysc.category1 = 0x3; 
	ArrowPhysc.category2 = 0x0; 
	ArrowPhysc.bitfield1 = 0x4F; 
	ArrowPhysc.bitfield2 = 0xFFFFFFFF; 
	ArrowPhysc.unkShort1C = 0; 
	ArrowPhysc.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &ArrowPhysc); 
	this->aPhysics.addToList();

	// Tile collider

	// These fucking rects do something for the tile rect
	/*
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;
	*/
	
	static const pointSensor_s below(0<<12, 0<<12);
	static const pointSensor_s above(0<<12, 24<<12);
	static const lineSensor_s adjacent(12<<12, 18<<12, 16<<12);
	
	bindAnimChr_and_setUpdateRate("kanban_yajirushi", 1, 0.0, 1.0);

	if (affectedByGravity) {doStateChange(&StateID_Fall);}
	
	this->onExecute();

	return true;
}

int daKanbanYajirushi::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();
	acState.execute();

	if (isMessageArrow) {}
	else{this->animationChr.setCurrentFrame(frame);}

	return true;
}

int daKanbanYajirushi::onDelete() {
	return true;
}

int daKanbanYajirushi::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daKanbanYajirushi::beginState_Fall() {
	belowSensor.flags = SENSOR_LINE;
	//belowSensor.flags |= SENSOR_10000000 | SENSOR_BREAK_BLOCK | SENSOR_BREAK_BRICK;
	// 10000000 makes it pass through bricks

	belowSensor.lineA = -24 << 12;
	belowSensor.lineB = 24 << 12;
	belowSensor.distanceFromCenter = 0;

	collMgr.init(this, &belowSensor, 0, 0);

	speed.y = 0.0f;
	y_speed_inc = -0.1875f;
	max_speed.y = -4.0f;
}
void daKanbanYajirushi::executeState_Fall() {
	HandleYSpeed();
	doSpriteMovement();
	UpdateObjectPosBasedOnSpeedValuesReal();

	if (collMgr.calculateBelowCollision() & (~0x400000)) {
		
		if (this->efTimer == 0) {
			Vec efPos = {this->pos.x + 0, this->pos.y - 16.0f, 5500.0f};
			S16Vec nullRot = {0,0,0};
			Vec oneVec = {scale.x, scale.y, 1.0f};
			SpawnEffect("Wm_mr_landsmoke", 0, &efPos, &nullRot, &oneVec);
		}
		
		this->efTimer++;
		
	} else {
		efTimer = 0;
	}
}
void daKanbanYajirushi::endState_Fall() {}