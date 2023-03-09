#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>


const char* MCPowerSarcList [] = { "MC_powerup", NULL };

class daMinecartObj_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daMinecartObj_c* build();

	int power;
	int timer;
	int newTimer;
	u32 enitemsettings;
	static u32 reggieToIG[];
	u32 powerupToSet;
	int playerID;
	bool isCaught;
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	m3d::anmTexPat_c animationPat;
	nw4r::g3d::ResFile resFile;

};


void daMinecartObj_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daMinecartObj_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daMinecartObj_c* daMinecartObj_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMinecartObj_c));
	return new(buffer) daMinecartObj_c;
}

void daMinecartObj_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	
	dAcPy_c* player = (dAcPy_c*)apOther->owner;
	isCaught = true;
	
	if (this->newTimer == 0) {
		
		Vec efPos = {pos.x + 0.0f, pos.y - 2.0f, pos.z + 100.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {2.0f, 0.0f, 2.0f};
		SpawnEffect("Wm_en_burst_s", 0, &efPos, &nullRot, &efScale);
		
		PlaySound(this, SE_PLY_BALLOON_BRAKE);
		
		u32 enitemsettings; //EN_ITEM settings
		u32 reggieToIG[] = {0x0,0x1,0x2,0x7,0x9,0xE,0x11,0x15,0x19,0x6,0x2}; //Mushroom, Star, Coin, 1UP, Fire Flower, Ice Flower, Penguin, Propeller, Mini Shroom, Hammer, 10 Coins
		powerupToSet = reggieToIG[(this->settings >> 16 & 0xF)]; //Spritedata ID to EN_ITEM ID
		enitemsettings = 0 | (powerupToSet << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (this->playerID + 8 << 16); //Setting non-GP settings
	
		dStageActor_c *power = dStageActor_c::create(EN_ITEM, enitemsettings, &player->pos, 0, 0);
		
	}
	
	this->Delete(1);
	
}

void daMinecartObj_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void daMinecartObj_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

bool daMinecartObj_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daMinecartObj_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daMinecartObj_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daMinecartObj_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool daMinecartObj_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool daMinecartObj_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool daMinecartObj_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool daMinecartObj_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

int daMinecartObj_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("MC_powerup", "g3d/MC_powerup.brres");
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("MC_powerup");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Item(&bodyModel, 0);
	
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	
	nw4r::g3d::ResAnmChr anmB = this->resFile.GetResAnmTexPat("MG_balloon");
	this->animationPat.setup(mdl, &anmB, &this->allocator, 0, 1);

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	power = settings >> 16 & 0xF; //nybble 8
	this->timer = 0;
	this->newTimer = 0;
	isCaught = false;
	
	ActivePhysics::Info EvilBalloon; 
	EvilBalloon.xDistToCenter = 0.0; 
	EvilBalloon.yDistToCenter = 2.0; 
	EvilBalloon.xDistToEdge = 16.0; 
	EvilBalloon.yDistToEdge = 24.0; 
	EvilBalloon.category1 = 0x3; 
	EvilBalloon.category2 = 0x0; 
	EvilBalloon.bitfield1 = 0x4F; 
	EvilBalloon.bitfield2 = 0xFFFFFFFF; 
	EvilBalloon.unkShort1C = 0; 
	EvilBalloon.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &EvilBalloon); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	this->onExecute();

	return true;
}

int daMinecartObj_c::onExecute() {
	updateModelMatrices();
	if (!isCaught) {bodyModel._vf1C();}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}
	
	if ((timer == 25) && (!isCaught)) {
		Vec efPos = {pos.x + 0.1f, pos.y + 0.1f, pos.z + 800.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {3.0f, 3.0f, 1.0f};
		SpawnEffect("Wm_ob_itemappear", 0, &efPos, &nullRot, &efScale);
	}
	
	animationPat.setCurrentFrame(power);
	
	this->timer++;
	
	return true;
}

int daMinecartObj_c::onDelete() {
	return true;
}

int daMinecartObj_c::onDraw() {
	if (!isCaught) {bodyModel.scheduleForDrawing();}

	return true;
}