#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
//#include "boss.h"
#include <profile.h>

//#define cPlayerOccupying (*(dStageActor_c**)(((u32)(clown)) + 0x738 ))
//Can this order be used again? (Comes From bossPlayerClown)

const char* MoonSArcList [] = { "I_moon", NULL };

//class daEnItem_c : public dEn_c {};

class daLiveMoon_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	
	int timer;
	int areLivesCreated;
	bool isTouched;

	daEnItem_c *OneUp0;
	daEnItem_c *OneUp1;
	daEnItem_c *OneUp2;	
	
	dStageActor_c *Live;
	dStageActor_c *Live1;
	dStageActor_c *Live2;

	/* Make sure to use the correct position
	Vec OneUpPos = (Vec){this->pos.x, this->pos.y, this->pos.z};
	S16Vec OneUpRot = (S16Vec){0, 0, 0};
	
	// Create And use Invisible 1-UP Mushroom
	OneUp0 = (daEnItem_c*)createChild(EN_ITEM, (dStageActor_c*)this, 0, &OneUpPos, &OneUpRot, 0);
	OneUp0->powerup = 7 & 0x1F; //according to NSMBW NewerRes "switch7_1up"
	OneUp0->pos.x = this->pos.x - 16.0;
	OneUp0->pos.y = this->pos.y + 8.0;
			
	OneUp0->scale.x = 0.0;
	OneUp0->scale.y = 0.0;
	OneUp0->scale.z = 0.0;
	
	// Create And use Invisible 1-UP Mushroom
	OneUp1 = (daEnItem_c*)createChild(EN_ITEM, (dStageActor_c*)this, 0, &OneUpPos, &OneUpRot, 0);
	OneUp1->powerup = 7 & 0x1F; //according to NSMBW NewerRes "switch7_1up"
	OneUp1->pos.x = this->pos.x - 16.0;
	OneUp1->pos.y = this->pos.y + 8.0;
			
	OneUp1->scale.x = 0.0;
	OneUp1->scale.y = 0.0;
	OneUp1->scale.z = 0.0;
	
	// Create And use Invisible 1-UP Mushroom
	OneUp2 = (daEnItem_c*)createChild(EN_ITEM, (dStageActor_c*)this, 0, &OneUpPos, &OneUpRot, 0);
	OneUp2->powerup = 7 & 0x1F; //according to NSMBW NewerRes "switch7_1up"
	OneUp2->pos.x = this->pos.x - 16.0;
	OneUp2->pos.y = this->pos.y + 8.0;
			
	OneUp2->scale.x = 0.0;
	OneUp2->scale.y = 0.0;
	OneUp2->scale.z = 0.0; */
	
	static dStageActor_c *createChild(Actors type, dStageActor_c *parent, u32 settings, Vec *pos, S16Vec *rot, u8 layer);

	static dActor_c* build();
	dAcPy_c *player;

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther); 
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daLiveMoon_c);
	DECLARE_STATE(Wait);
};

extern "C" nw4r::snd::SoundHandle *PlaySound(dStageActor_c *, int soundID);

const SpriteData LIVE_MOONSpriteData = { ProfileId::LIVE_MOON, 0, 0, -8, 8, 0x100, 0x100, 0, 0, 0, 0, 0 };

Profile LIVE_MOONProfile(&daLiveMoon_c::build, SpriteId::LIVE_MOON, &LIVE_MOONSpriteData, ProfileId::LIVE_MOON, ProfileId::LIVE_MOON, "LIVE_MOON", MoonSArcList);

CREATE_STATE(daLiveMoon_c, Wait);

void daLiveMoon_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

dActor_c* daLiveMoon_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daLiveMoon_c));
	return new(buffer) daLiveMoon_c;
}


void daLiveMoon_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->isTouched = true;
	this->pos.x = 330.0;
	this->pos.y = 240.0;
	
	int enitemsettings = 0 | (7 << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (8 << 16);  //setting the settings
	Live = dStageActor_c::create(EN_ITEM, enitemsettings, &player->pos, 0, 0);
	Live1 = dStageActor_c::create(EN_ITEM, enitemsettings, &player->pos, 0, 0);
	Live2 = dStageActor_c::create(EN_ITEM, enitemsettings, &player->pos, 0, 0);
	Live->scale.x = 0.0;
	Live1->scale.x = 0.0;
	Live2->scale.x = 0.0;
	
	Vec efPos = {pos.x + 8.0f, pos.y + 8.0f, pos.z};
	S16Vec nullRot = {0,0,0};
	Vec efScale = {2.0f, 2.0f, 2.0f};
	SpawnEffect("Wm_ob_keyget02_hit", 0, &efPos, &nullRot, &efScale);
}

void daLiveMoon_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	player = dAcPy_c::findByID(0);; //ActivePhysics *apOther;
	//Vec tempPos; (Intended use unknown)
	
	Vec efPos4 = {pos.x + 8.0f, pos.y + 8.0f, pos.z};
	S16Vec nullRot4 = {0,0,0};
	Vec efScale4 = {2.0f, 2.0f, 2.0f};
	SpawnEffect("Wm_ob_keyget02_hit", 0, &efPos4, &nullRot4, &efScale4);

	/*
	int TouchedPlayer = player; //Is it gonna give us player's ID?
	for (int ii = 0; ii < 3; ii++) {//Syntax?
		Player_Lives[player] += 1;
		PlaySound(this, SE_SYS_ONE_UP);
	}
	*/
	
	
	if ((player = dAcPy_c::findByID(1)) && (!isTouched)) {PlaySound(this, SE_VOC_LU_CLEAR_BOSS);} //I didn't care if counter starts at 0 or 1.
	else if ((player = dAcPy_c::findByID(2)) && (!isTouched)) {PlaySound(this, SE_VOC_KO_CLEAR_BOSS);}
	else if ((player = dAcPy_c::findByID(3)) && (!isTouched)) {PlaySound(this, SE_VOC_KO2_CLEAR_BOSS);}
	else if ((player = dAcPy_c::findByID(0)) && (!isTouched)) {PlaySound(this, SE_VOC_MA_CLEAR_BOSS);}
	
	this->onDelete();
}

void daLiveMoon_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {}

bool daLiveMoon_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daLiveMoon_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daLiveMoon_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daLiveMoon_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

bool daLiveMoon_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}


int daLiveMoon_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("I_moon", "g3d/I_moon.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("I_moon");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);

	allocator.unlink(); 
	
	this->isTouched = false;
	this->direction = 0; //On the right (no much utilities XD)
	
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info moonPhysics; 
	moonPhysics.xDistToCenter = 0.0; 
	moonPhysics.yDistToCenter = 8.0; 
	moonPhysics.xDistToEdge = 12.0; 
	moonPhysics.yDistToEdge = 8.0; 
	moonPhysics.category1 = 0x3; 
	moonPhysics.category2 = 0x0; 
	moonPhysics.bitfield1 = 0x4F; 
	moonPhysics.bitfield2 = 0xFFFFFFFF; 
	//Ground Pound Yoshi Physics can't be found in NewSpriteCreator :(
	moonPhysics.unkShort1C = 0; 
	moonPhysics.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &moonPhysics); 
	this->aPhysics.addToList();


	areLivesCreated = 0;
	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int daLiveMoon_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}

int daLiveMoon_c::onDelete() {
	return true;
}

int daLiveMoon_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daLiveMoon_c::beginState_Wait() {
	this->timer = 0;
	PlaySound(this, SE_OBJ_BROOM_KEY_APP);
	areLivesCreated++;
}
void daLiveMoon_c::executeState_Wait() {
	
	/* if ((timer == 0) && (areLivesCreated == 0)) {
		//for (int k = 0; k < 3; k++) { Unfortunately, loops are forbidden while making pointers
		// Make sure to use the correct position
		Vec OneUpPos = (Vec){this->pos.x, this->pos.y, this->pos.z};
		S16Vec OneUpRot = (S16Vec){0, 0, 0};
	
		// Create And use Invisible 1-UP Mushroom
		OneUp0 = (daEnItem_c*)createChild(EN_ITEM, (dStageActor_c*)this, 0, &OneUpPos, &OneUpRot, 0);
		OneUp0->powerup = 7 & 0x1F; //according to NSMBW NewerRes "switch7_1up"

		OneUp0->scale.x = 1.0;
		OneUp0->scale.y = 1.0;
		OneUp0->scale.z = 1.0;
		
		// Create And use Invisible 1-UP Mushroom
		OneUp1 = (daEnItem_c*)createChild(EN_ITEM, (dStageActor_c*)this, 0, &OneUpPos, &OneUpRot, 0);
		OneUp1->powerup = 7 & 0x1F; //according to NSMBW NewerRes "switch7_1up"	
		
		OneUp1->scale.x = 1.0;
		OneUp1->scale.y = 1.0;
		OneUp1->scale.z = 1.0;
		
		// Create And use Invisible 1-UP Mushroom
		OneUp2 = (daEnItem_c*)createChild(EN_ITEM, (dStageActor_c*)this, 0, &OneUpPos, &OneUpRot, 0);
		OneUp2->powerup = 7 & 0x1F; //according to NSMBW NewerRes "switch7_1up"

			
		OneUp2->scale.x = 1.0;
		OneUp2->scale.y = 1.0;
		OneUp2->scale.z = 1.0;
		//}
	}
	
	OneUp0->pos.x = this->pos.x - 16.0;
	OneUp0->pos.y = this->pos.y + 8.0;
	OneUp1->pos.x = this->pos.x - 16.0;
	OneUp1->pos.y = this->pos.y + 8.0;
	OneUp2->pos.x = this->pos.x - 16.0;
	OneUp2->pos.y = this->pos.y + 8.0;
	
		for (int j = 0; j < 60; j++) {
		this->pos.y += 0.05;
		//Why not two nummbers after the point? Does it work in this case?
		if (( j <= 30) && (direction == 0)) {
			this->direction = 0;
			this->rot.y += 0x0050;
		} else {
			this->direction = 0;
			this->rot.y -= 0x0050;
		}
	} */
	
	this->rot.y += 0x200;

	if ((timer == 1) && (!isTouched)) {
		Vec efPos2 = {pos.x + 8.0f, pos.y + 8.0f, pos.z};
		S16Vec nullRot2 = {0,0,0};
		Vec efScale2 = {2.0f, 2.0f, 2.0f};	
		SpawnEffect("Wm_ob_keywait", 0, &efPos2, &nullRot2, &efScale2);
	}

	if ((timer == 60) && (!isTouched)) {
		Vec efPos3 = {pos.x + 8.0f, pos.y + 8.0f, pos.z};
		S16Vec nullRot3 = {0,0,0};
		Vec efScale3 = {2.0f, 2.0f, 2.0f};
		SpawnEffect("Wm_ob_keywait_c", 0, &efPos3, &nullRot3, &efScale3);
	}
	
	this->timer++;
	
}
void daLiveMoon_c::endState_Wait() {}
