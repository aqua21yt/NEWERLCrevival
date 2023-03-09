#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <stage.h>
#include <profile.h>

const char* HitboxArrowSarcList [] = {
	"kanban_yajirushi",
	"kanban_yajirushi_message",
	NULL };

class daHitboxArrow_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	void accordingToAaronMallieItDidntFallFirst();
	void ShoutWheneverYouWant();
	
	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrB_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrC_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrD_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrE_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModelsAndAnims();

	int SignType;
	int UsedModel;
	int DisplayMessageID;
	int timer;
	int delay;
	
	static dActor_c* build();

	//void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

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


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c newModelA;
	m3d::mdl_c newModelB;
	m3d::mdl_c newModelC;
	m3d::mdl_c newModelD;
	m3d::mdl_c newModelE;
	m3d::anmChr_c animationChr;
	m3d::anmChr_c animationChrA;
	m3d::anmChr_c animationChrB;
	m3d::anmChr_c animationChrC;
	m3d::anmChr_c animationChrD;
	m3d::anmChr_c animationChrE;
	nw4r::g3d::ResFile resFile;

};

//---------FALLING STATE--------//
void daHitboxArrow_c::accordingToAaronMallieItDidntFallFirst() {
	
	if (!collMgr.isOnTopOfTile()) {this->rot.y -= 7.0;}
	else {this->rot.y += 1.0;}
	
}

#include "msgbox.h"
void daHitboxArrow_c::ShoutWheneverYouWant() {
	/* &dStarSign_c::build; //Oh oh , not sure!!!!
	dStarSign_c::WriteBMGToTextBox(layout.findTextBoxByName("T_message_00"), GetBMG(), 0x28160294 + DisplayMessageID, 0, 0); */
	
	if (this->timer == 1) {dMsgBoxManager_c::instance->showMessage(DisplayMessageID, true, delay);}
	
	if (this->timer == 120) {this->timer = 0;}
	
	this->timer++;
}

void daHitboxArrow_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
	
	newModelA.setDrawMatrix(matrix);
	newModelA.setScale(&scale);
	newModelA.calcWorld(false);
	
	newModelB.setDrawMatrix(matrix);
	newModelB.setScale(&scale);
	newModelB.calcWorld(false);
	
	newModelC.setDrawMatrix(matrix);
	newModelC.setScale(&scale);
	newModelC.calcWorld(false);
	
	newModelD.setDrawMatrix(matrix);
	newModelD.setScale(&scale);
	newModelD.calcWorld(false);
	
	newModelE.setDrawMatrix(matrix); //modèle
	newModelE.setScale(&scale);
	newModelE.calcWorld(false);
}

void daHitboxArrow_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void daHitboxArrow_c::bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChrA = this->resFile.GetResAnmChr(name);
	this->animationChrA.bind(&this->newModelA, anmChrA, unk);
	this->newModelA.bindAnim(&this->animationChrA, unk2);
	this->animationChrA.setUpdateRate(rate);
}

void daHitboxArrow_c::bindAnimChrB_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChrB = this->resFile.GetResAnmChr(name);
	this->animationChrB.bind(&this->newModelB, anmChrB, unk);
	this->newModelB.bindAnim(&this->animationChrB, unk2);
	this->animationChrB.setUpdateRate(rate);
}

void daHitboxArrow_c::bindAnimChrC_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChrC = this->resFile.GetResAnmChr(name);
	this->animationChrC.bind(&this->newModelC, anmChrC, unk);
	this->newModelC.bindAnim(&this->animationChrC, unk2);
	this->animationChrC.setUpdateRate(rate);
}

void daHitboxArrow_c::bindAnimChrD_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChrD = this->resFile.GetResAnmChr(name);
	this->animationChrD.bind(&this->newModelD, anmChrD, unk);
	this->newModelD.bindAnim(&this->animationChrD, unk2);
	this->animationChrD.setUpdateRate(rate);
}

void daHitboxArrow_c::bindAnimChrE_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChrE = this->resFile.GetResAnmChr(name);
	this->animationChrE.bind(&this->newModelE, anmChrE, unk);
	this->newModelE.bindAnim(&this->animationChrE, unk2);
	this->animationChrE.setUpdateRate(rate);
}

void daHitboxArrow_c::setupModelsAndAnims() {
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	//Intended use is unknown.

	resFile.data = getResource("kanban_yajirushi_message", "g3d/kanban_yajirushi.brres");
	
	if (UsedModel == 1) {
		if (SignType == 1) {
			
			nw4r::g3d::ResMdl mdlB = this->resFile.GetResMdl("kanban_yajirushi_1");
			this->newModelB.setup(mdlB, &allocator, 0x224, 1, 0);
			SetupTextures_Item(&this->newModelB, 0);
			
			nw4r::g3d::ResAnmChr anmChrB = this->resFile.GetResAnmChr("kanban_yajirushi");
			this->animationChrB.setup(mdlB, anmChrB, &this->allocator, 0);
		} else if (SignType == 2) {
			
			nw4r::g3d::ResMdl mdlC = this->resFile.GetResMdl("kanban_yajirushi_2");
			this->newModelC.setup(mdlC, &allocator, 0x224, 1, 0);
			SetupTextures_Item(&this->newModelC, 0);
			
			nw4r::g3d::ResAnmChr anmChrC = this->resFile.GetResAnmChr("kanban_yajirushi");
			this->animationChrC.setup(mdlC, anmChrC, &this->allocator, 0);
		} else if (SignType == 3) {
			
			nw4r::g3d::ResMdl mdlD = this->resFile.GetResMdl("kanban_yajirushi_3");
			this->newModelD.setup(mdlD, &allocator, 0x224, 1, 0);
			SetupTextures_Item(&this->newModelD, 0);
			
			nw4r::g3d::ResAnmChr anmChrD = this->resFile.GetResAnmChr("kanban_yajirushi");
			this->animationChrD.setup(mdlD, anmChrD, &this->allocator, 0);
		} else if (SignType == 4) {
			
			nw4r::g3d::ResMdl mdlE = this->resFile.GetResMdl("kanban_yajirushi_4");
			this->newModelE.setup(mdlE, &allocator, 0x224, 1, 0);
			SetupTextures_Item(&this->newModelE, 0);
			
			nw4r::g3d::ResAnmChr anmChrE = this->resFile.GetResAnmChr("kanban_yajirushi");
			this->animationChrE.setup(mdlE, anmChrE, &this->allocator, 0);
		} else {
			
			nw4r::g3d::ResMdl mdlA = this->resFile.GetResMdl("kanban_yajirushi_0");
			this->newModelA.setup(mdlA, &allocator, 0x224, 1, 0);
			SetupTextures_Item(&this->newModelA, 0);
			
			nw4r::g3d::ResAnmChr anmChrA = this->resFile.GetResAnmChr("kanban_yajirushi");
			this->animationChrA.setup(mdlA, anmChrA, &this->allocator, 0);
		}
	} else {
		
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("kanban_yajirushi");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Item(&bodyModel, 0);
		
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("kanban_yajirushi");
		this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	}
	
}

dActor_c* daHitboxArrow_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daHitboxArrow_c));
	return new(buffer) daHitboxArrow_c;
}

const SpriteData HitboxArrowSignboardSpriteData = { ProfileId::HitboxArrowSignboard, 0, 0, 0, 0, 0x200, 0x200, 0, 0, 0, 0, 0 };

Profile HitboxArrowSignboardProfile(&daHitboxArrow_c::build, SpriteId::HitboxArrowSignboard, &HitboxArrowSignboardSpriteData, ProfileId::HitboxArrowSignboard, ProfileId::HitboxArrowSignboard, "HitboxArrowSignboard", HitboxArrowSarcList);

void daHitboxArrow_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->timer = 0;
}

int daHitboxArrow_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	setupModelsAndAnims();
	
	UsedModel = settings >> 4 & 0xF000000; //nybble 11
	SignType = settings & 0xF000000; //nybble 12
	DisplayMessageID = settings >> 8 & 0xFFF0000; //nybble 8-10
	delay = settings >> 20 & 0xFFF0000; //nybbles 5-7


	ActivePhysics::Info IlikeGunsXD; 
	IlikeGunsXD.xDistToCenter = 0.0; 
	IlikeGunsXD.yDistToCenter = 24.0; 
	IlikeGunsXD.xDistToEdge = 24.0; 
	IlikeGunsXD.yDistToEdge = 24.0; 
	IlikeGunsXD.category1 = 0x3; 
	IlikeGunsXD.category2 = 0x0; 
	IlikeGunsXD.bitfield1 = 0x4F; 
	IlikeGunsXD.bitfield2 = 0xFFFFFFFF; 
	IlikeGunsXD.unkShort1C = 0; 
	IlikeGunsXD.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &IlikeGunsXD); 
	this->aPhysics.addToList();

	allocator.unlink(); 
	
	
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	bindAnimChr_and_setUpdateRate("kanban_yajirushi", 1, 0.0, 1.0);

	this->onExecute();

	return true;
}

int daHitboxArrow_c::onExecute() {
	updateModelMatrices();
	if (UsedModel == 1) {
		if (SignType == 1) {newModelB._vf1C();}
		else if (SignType == 2) {newModelC._vf1C();}
		else if (SignType == 3) {newModelD._vf1C();}
		else if (SignType == 4) {newModelE._vf1C();}
		else {newModelA._vf1C();}
	} else {
		
		bodyModel._vf1C();

		
	}
	
	collMgr.calculateBelowCollisionWithSmokeEffect();
	this->animationChr.setCurrentFrame(SignType);
	//Mistake or not?
	
	return true;
}

int daHitboxArrow_c::onDelete() {
	return true;
}

int daHitboxArrow_c::onDraw() {
	if (UsedModel == 1) {
		if (SignType == 1) {newModelB.scheduleForDrawing();}
		else if (SignType == 2) {newModelC.scheduleForDrawing();}
		else if (SignType == 3) {newModelD.scheduleForDrawing();}
		else if (SignType == 4) {newModelE.scheduleForDrawing();}
		else {newModelA.scheduleForDrawing();}
	} else {bodyModel.scheduleForDrawing();}

	return true;
}


//Wow, that was easy for once. Isn't it?