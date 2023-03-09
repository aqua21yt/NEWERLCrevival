#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include <profile.h>


const char* FlagsSarcContent [] = {
	"middle_flag",
	"goal_set",
	"star_coin",
	NULL
};

class daTrickPostFlag_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModels();
	
	int Type;
	int Object;
	bool HasAShitHouse;

	static dActor_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
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
	//Setup models - Phase 1:
	m3d::mdl_c flagModel;
	m3d::mdl_c midwayModel;
	m3d::mdl_c towerModel;
	m3d::mdl_c coinModel;
	m3d::mdl_c baseModel;
	
	m3d::anmChr_c animationChr;
	m3d::anmChr_c animationChrA;
	nw4r::g3d::ResFile resFile;

};


void daTrickPostFlag_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	midwayModel.setDrawMatrix(matrix);
	midwayModel.setScale(&scale);
	midwayModel.calcWorld(false);
	
	baseModel.setDrawMatrix(matrix);
	baseModel.setScale(&scale);
	baseModel.calcWorld(false);
	
	towerModel.setDrawMatrix(matrix);
	towerModel.setScale(&scale);
	towerModel.calcWorld(false);
	
	coinModel.setDrawMatrix(matrix);
	coinModel.setScale(&scale);
	coinModel.calcWorld(false);
	
	flagModel.setDrawMatrix(matrix);
	flagModel.setScale(&scale);
	flagModel.calcWorld(false);
	
}


void daTrickPostFlag_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) { //For midway flag
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->midwayModel, anmChr, unk);
	this->midwayModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void daTrickPostFlag_c::bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate) { //For flag pole
	nw4r::g3d::ResAnmChr anmA = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->flagModel, anmA, unk);
	this->flagModel.bindAnim(&this->animationChrA, unk2);
	this->animationChrA.setUpdateRate(rate);
}

void daTrickPostFlag_c::setupModels() {

	if (Object == 1) { //The object is a fake goal pole
	
		if (Type == 1) {resFile.data = getResource("goal_set", "g3d/t01.brres");}
		else if (Type == 2) {resFile.data = getResource("goal_set", "g3d/t02.brres");}
		else if (Type == 3) {resFile.data = getResource("goal_set", "g3d/t03.brres");}
		else if (Type == 4) {resFile.data = getResource("goal_set", "g3d/t04.brres");}
		else if (Type == 5) {resFile.data = getResource("goal_set", "g3d/t05.brres");}
		else if (Type == 6) {resFile.data = getResource("goal_set", "g3d/t06.brres");}
		else if (Type == 7) {resFile.data = getResource("goal_set", "g3d/t07.brres");}
		else if (Type == 8) {resFile.data = getResource("goal_set", "g3d/t08.brres");}
		else if (Type == 9) {resFile.data = getResource("goal_set", "g3d/t09.brres");}
		else if (Type == 10) {resFile.data = getResource("goal_set", "g3d/t10.brres");}
		else if (Type == 11) {resFile.data = getResource("goal_set", "g3d/t11.brres");}
		else if (Type == 12) {resFile.data = getResource("goal_set", "g3d/t12.brres");}
		else if (Type == 13) {resFile.data = getResource("goal_set", "g3d/t13.brres");}
		else if (Type == 14) {resFile.data = getResource("goal_set", "g3d/t14.brres");}
		else if (Type == 15) {resFile.data = getResource("goal_set", "g3d/t15.brres");}
		else {resFile.data = getResource("goal_set", "g3d/t00.brres");}
		
		nw4r::g3d::ResMdl mdlA = this->resFile.GetResMdl("baseA");
		baseModel.setup(mdlA, &allocator, 0x224, 1, 0);
		SetupTextures_MapObj(&baseModel, 0);
		
		nw4r::g3d::ResMdl mdlB = this->resFile.GetResMdl("goal_flag");
		flagModel.setup(mdlB, &allocator, 0x224, 1, 0);
		SetupTextures_MapObj(&flagModel, 0);
		
		//flagModel->pos.y = baseModel->pos.y + 16.0;
		
		nw4r::g3d::ResAnmChr anmA = this->resFile.GetResAnmChr("wait");
		this->animationChrA.setup(mdlB, anmA, &this->allocator, 0);
		
		if (HasAShitHouse) {
			
			nw4r::g3d::ResMdl mdlC = this->resFile.GetResMdl("toride_std");
			towerModel.setup(mdlC, &allocator, 0x224, 1, 0);
			SetupTextures_MapObj(&towerModel, 0);
			
		}
		
	} else if (Object == 2) { //The object is a fake star coin

		if (Type == 1) {resFile.data = getResource("star_coin", "g3d/t01.brres");}
		else if (Type == 2) {resFile.data = getResource("star_coin", "g3d/t02.brres");}
		else if (Type == 3) {resFile.data = getResource("star_coin", "g3d/t03.brres");}
		else if (Type == 4) {resFile.data = getResource("star_coin", "g3d/t04.brres");}
		else if (Type == 5) {resFile.data = getResource("star_coin", "g3d/t05.brres");}
		else if (Type == 6) {resFile.data = getResource("star_coin", "g3d/t06.brres");}
		else if (Type == 7) {resFile.data = getResource("star_coin", "g3d/t07.brres");}
		else if (Type == 8) {resFile.data = getResource("star_coin", "g3d/t08.brres");}
		else if (Type == 9) {resFile.data = getResource("star_coin", "g3d/t09.brres");}
		else if (Type == 10) {resFile.data = getResource("star_coin", "g3d/t10.brres");}
		else if (Type == 11) {resFile.data = getResource("star_coin", "g3d/t11.brres");}
		else if (Type == 12) {resFile.data = getResource("star_coin", "g3d/t12.brres");}
		else if (Type == 13) {resFile.data = getResource("star_coin", "g3d/t13.brres");}
		else if (Type == 14) {resFile.data = getResource("star_coin", "g3d/t14.brres");}
		else if (Type == 15) {resFile.data = getResource("star_coin", "g3d/t15.brres");}
		else {resFile.data = getResource("star_coin", "g3d/t00.brres");}

		nw4r::g3d::ResMdl mdlD = this->resFile.GetResMdl("star_coinA");
		coinModel.setup(mdlD, &allocator, 0x224, 1, 0);
		SetupTextures_MapObj(&coinModel, 0);		

	} else { //The object is a midway flag
		
		if (Type == 1) {resFile.data = getResource("middle_flag", "g3d/t01.brres");}
		else if (Type == 2) {resFile.data = getResource("middle_flag", "g3d/t02.brres");}
		else if (Type == 3) {resFile.data = getResource("middle_flag", "g3d/t03.brres");}
		else if (Type == 4) {resFile.data = getResource("middle_flag", "g3d/t04.brres");}
		else if (Type == 5) {resFile.data = getResource("middle_flag", "g3d/t05.brres");}
		else if (Type == 6) {resFile.data = getResource("middle_flag", "g3d/t06.brres");}
		else if (Type == 7) {resFile.data = getResource("middle_flag", "g3d/t07.brres");}
		else if (Type == 8) {resFile.data = getResource("middle_flag", "g3d/t08.brres");}
		else if (Type == 9) {resFile.data = getResource("middle_flag", "g3d/t09.brres");}
		else if (Type == 10) {resFile.data = getResource("middle_flag", "g3d/t10.brres");}
		else if (Type == 11) {resFile.data = getResource("middle_flag", "g3d/t11.brres");}
		else if (Type == 12) {resFile.data = getResource("middle_flag", "g3d/t12.brres");}
		else if (Type == 13) {resFile.data = getResource("middle_flag", "g3d/t13.brres");}
		else if (Type == 14) {resFile.data = getResource("middle_flag", "g3d/t14.brres");}
		else if (Type == 15) {resFile.data = getResource("middle_flag", "g3d/t15.brres");}
		else {resFile.data = getResource("middle_flag", "g3d/t00.brres");}
	
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("middle_flag");
		midwayModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_MapObj(&midwayModel, 0);
	
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
		this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
		
	}
	
}

void daTrickPostFlag_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

	PlaySound(this, SE_EMY_CS_TERESA_BEAT_YOU);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_en_obakedoor", 0, &this->pos, &nullRot, &oneVec);

	//FIXME changed to dStageActor_c::Delete(u8) from fBase_c::Delete(void)
	this->Delete(1);
}

bool daTrickPostFlag_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTrickPostFlag_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTrickPostFlag_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTrickPostFlag_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}

bool daTrickPostFlag_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_en_explosion", 0, &this->pos, &nullRot, &oneVec);
	Vec smokeScale = {3.0f, 3.0f, 3.0f};
	SpawnEffect("Wm_en_explosion_smk", 0, &this->pos, &nullRot, &smokeScale);

	PlaySound(this, SE_OBJ_EMY_FIRE_DISAPP);
	this->Delete(1);

	return true;
}
bool daTrickPostFlag_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	S16Vec nullRot = {0,0,0};
	Vec scale15 = {1.5f, 1.5f, 1.5f};
	SpawnEffect("Wm_ob_cmnicekira", 0, &this->pos, &nullRot, &scale15);
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_ob_icebreakwt", 0, &this->pos, &nullRot, &oneVec);
	SpawnEffect("Wm_ob_iceattack", 0, &this->pos, &nullRot, &scale15);

	PlaySound(this, SE_OBJ_PNGN_ICE_BREAK);

	this->Delete(1);
	return true;
}
bool daTrickPostFlag_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_ob_cmnboxgrain", 0, &this->pos, &nullRot, &oneVec);
	SpawnEffect("Wm_en_obakedoor_sm", 0, &this->pos, &nullRot, &oneVec);

	PlaySound(this, SE_BOSS_JR_FLOOR_BREAK);

	//FIXME changed to dStageActor_c::Delete(u8) from fBase_c::Delete(void)
	this->Delete(1);
	return true;
}
bool daTrickPostFlag_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTrickPostFlag_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daTrickPostFlag_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}

dActor_c* daTrickPostFlag_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daTrickPostFlag_c));
	return new(buffer) daTrickPostFlag_c;
}

const SpriteData TrickyFlagSpriteData = { ProfileId::TrickyFlag, 0, 0, 0, 0, 0x150, 0x300, 0, 0, 0, 0, 0 };

Profile TrickyFlagProfile(&daTrickPostFlag_c::build, SpriteId::TrickyFlag, &TrickyFlagSpriteData, ProfileId::TrickyFlag, ProfileId::TrickyFlag, "TrickyFlag", FlagsSarcContent);

int daTrickPostFlag_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	Type = settings >> 24 & 0xF; //nybble 6
	Object = settings >> 12 & 0xF; //nybble 9
	HasAShitHouse = settings >> 8 & 0xF; //nybble 10

	setupModels();
		

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;
	
	if (Object != 2) {this->rot.y = 0x200;}


	ActivePhysics::Info KindAndCoolPhysics;

	if (Object == 1) { // = 10x1 rectangle
		KindAndCoolPhysics.xDistToCenter = 0.0; 
		KindAndCoolPhysics.yDistToCenter = 160.0; 
		KindAndCoolPhysics.xDistToEdge = 8.0; 
		KindAndCoolPhysics.yDistToEdge = 160.0; 
		KindAndCoolPhysics.category1 = 0x3; 
	} else if (Object == 2) { // = 2x2 square
		KindAndCoolPhysics.xDistToCenter = 0.0; 
		KindAndCoolPhysics.yDistToCenter = -3.0; 
		KindAndCoolPhysics.xDistToEdge = 12.0; 
		KindAndCoolPhysics.yDistToEdge = 15.0; 
		KindAndCoolPhysics.category1 = 0x5; 
	} else { //=3x1 rectangle
		KindAndCoolPhysics.xDistToCenter = 0.0; 
		KindAndCoolPhysics.yDistToCenter = 36.0; 
		KindAndCoolPhysics.xDistToEdge = 8.0; 
		KindAndCoolPhysics.yDistToEdge = 36.0; 
		KindAndCoolPhysics.category1 = 0x3; 
	}
	KindAndCoolPhysics.category2 = 0x0; 
	KindAndCoolPhysics.bitfield1 = 0x4F; 
	KindAndCoolPhysics.bitfield2 = 0xFFFFFFFF; 
	KindAndCoolPhysics.unkShort1C = 0; 
	KindAndCoolPhysics.callback = &dEn_c::collisionCallback; 
	
	this->aPhysics.initWithStruct(this, &KindAndCoolPhysics); 
	this->aPhysics.addToList();

	if (Object == 1) {bindAnimChrA_and_setUpdateRate("wait", 1, 0.0, 1.0);}
	else if (Object == 0) {bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);}

	this->onExecute();

	return true;
}

int daTrickPostFlag_c::onExecute() {
	updateModelMatrices();
	
	if (Object == 1) {
		baseModel._vf1C();
		flagModel._vf1C();
		if (HasAShitHouse) {towerModel._vf1C();}
	} else if (Object == 2) {coinModel._vf1C();}
	else {midwayModel._vf1C();}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	if (Object == 2) {
		this->pos.x = -120.0;
		this->pos.z = 0.0;
		this->rot.x += 0x200;
	}
	
	return true;
}

int daTrickPostFlag_c::onDelete() {
	return true;
}

int daTrickPostFlag_c::onDraw() {
	if (Object == 1) {
		baseModel.scheduleForDrawing();
		flagModel.scheduleForDrawing();
		if (HasAShitHouse) {towerModel.scheduleForDrawing();}
	} else if (Object == 2) {coinModel.scheduleForDrawing();}
	else {midwayModel.scheduleForDrawing();}

	return true;
}

// Important info: maybe SpriteTex function doesn't work for all the sprites selected in here. Though I'm able to program it for easy-programed sprites like those :)))))