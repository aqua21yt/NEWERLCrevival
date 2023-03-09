#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <profile.h>


const char* TrexSarcList [] = { "rex", NULL };

class daRex_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	
	char damage;
	int damageLimit;
	float max_speed_inc;
	float min_speed_inc;
	bool direction;
	int distance;
	bool stillFalling;
	u16 amt;
	u16 name;
	u32 cmgr_returnValue;
	char hitType;
	char isDown;
	char damageP;
	float downYpos;
	int timer;
	float EnemyBounceValue;
	bool ret;
	bool isBouncing;
	float xDelta;
	float max_speed;
	float x_speed_inc;
	float XSpeed;
	
	static const float deltas [];
	u32 props;
	float someFloat;
	
	bool willWalkOntoSuitableGround();
	bool calculateTileCollisions();
	void addScoreWhenHit(void *other);
	void powBlockActivated(bool isNotMPGP);
	
	static dActor_c* build();

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
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

	void calculateBelowCollisionWithSmokeEffect(); //HUH?
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daRex_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Stomp);
	DECLARE_STATE(Die);
	DECLARE_STATE(Turn);
	DECLARE_STATE(RealTurn);
};

//EXTERNS AND STATES

//extern "C" bool calculateTileCollisions();
extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);


CREATE_STATE(daRex_c, Walk);
CREATE_STATE(daRex_c, Stomp);
CREATE_STATE(daRex_c, Die);
CREATE_STATE(daRex_c, Turn);
CREATE_STATE(daRex_c, RealTurn);

void daRex_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daRex_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

bool daRex_c::willWalkOntoSuitableGround() {
	float deltas[] = {2.5f, -2.5f};
	VEC3 checkWhere = {
			pos.x + deltas[direction],
			4.0f + pos.y,
			pos.z};

	props = collMgr.getTileBehaviour2At(checkWhere.x, checkWhere.y, currentLayerID);

	//if (getSubType(props) == B_SUB_LEDGE)
	if (((props >> 16) & 0xFF) == 8)
		return false;

	someFloat = 0.0f;
	if (collMgr.sub_800757B0(&checkWhere, &someFloat, currentLayerID, 1, -1)) {
		if (someFloat < checkWhere.y && someFloat > (pos.y - 5.0f))
			return true;
	}

	return false;
}

bool daRex_c::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		// u32 blah = collMgr.s_80070760();
		// u8 one = (blah & 0xFF);
		// static const float incs[5] = {0.00390625f, 0.0078125f, 0.015625f, 0.0234375f, 0.03125f};
		// x_speed_inc = incs[one];
		max_speed = (direction == 1) ? -XSpeed : XSpeed;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daRex_c::addScoreWhenHit(void *other) { }
void daRex_c::powBlockActivated(bool isNotMPGP) { }

dActor_c* daRex_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daRex_c));
	return new(buffer) daRex_c;
}

const SpriteData T_REXSpriteData = { ProfileId::T_REX, 0, 0, 0, 0, 0x100, 0x200, 0, 0, 0, 0, 0 };

Profile T_REXProfile(&daRex_c::build, SpriteId::T_REX, &T_REXSpriteData, ProfileId::T_REX, ProfileId::T_REX, "T_REX", TrexSarcList);

void daRex_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	
	if (this->isDown == 0) {hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);}
	else {hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);}
	
	if(hitType == 1) {	// regular jump
		apOther->someFlagByte |= 2;
		if (this->isDown == 0) {
			this->playEnemyDownSound1();
			if (damage >= damageLimit * 2) {
				bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
				doStateChange(&StateID_Die);
			}
			else if (damage > damageLimit) {
				doStateChange(&StateID_Stomp);
				damage++;
			} else {
				doStateChange(&StateID_Walk);
				damage++;
			}
		} else if (hitType == 3) {	// spinning jump or whatever?
			apOther->someFlagByte |= 2;
			if (this->isDown == 0) {
				this->playEnemyDownSound1();
				
				if (damage >= 1) {
					doStateChange(&StateID_Die); 
				} else if (damage > damageLimit) {
					doStateChange(&StateID_Stomp);
					damage++;
				} else {
					doStateChange(&StateID_Walk);
					damage++;
				}
			} else { // Shy Guy is in downed mode - kill it with fire
				this->playEnemyDownSound1();
				doStateChange(&StateID_Die);
			}
		}
		else if(hitType == 0) {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		} else { // Shy Guy is in downed mode - kill it with fire
			this->playEnemyDownSound1();
			doStateChange(&StateID_Die);
		}
	}
}

void daRex_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	name = ((dEn_c*)apOther->owner)->name;
	
	if (name == EN_LIFT_REMOCON_TRPLN || name == EN_WHITE_BLOCK) {
		this->calculateBelowCollisionWithSmokeEffect();
		this->EnemyBounceValue;
	} else if (name == EN_JUMPDAI) {
		this->calculateBelowCollisionWithSmokeEffect();
		this->EnemyBounceValue;
	}
	
	dEn_c::spriteCollision(apThis, apOther);
}

void daRex_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
}

bool daRex_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	damage = damage + damageLimit;
	this->collisionCatD_Drill(apThis, apOther);
}

bool daRex_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	
	if (damage != damageLimit * 2) {
		damage = damageLimit * 2;
		this->timer = 0;
	}
	
	PlaySound(this, SE_EMY_DOWN);
	
	if (timer == 1) {
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_mr_hardhit", 0, &pos, &nullRot, &oneVec);
		aPhysics.removeFromList();
	}
	
	if (timer < 80) {
		this->downYpos = downYpos / 1.075;
		
		this->rot.x += 0x600;
		this->rot.y -= 0x900;
		this->rot.z += 0x100;
	} else {
		
		if (timer == 80) {this->downYpos = downYpos - downYpos * 2;} //+ becomes -
		
		this->downYpos = downYpos * 1.075;
		
		this->rot.x -= 0x300;
		this->rot.y += 0xA00;
		this->rot.z -= 0x500;
	}
	
	if (timer == 250) {this->Delete(1);}
	
	this->timer++;
}

bool daRex_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daRex_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->damage += damageLimit;
	
	S16Vec nullRot = {0,0,0};
	Vec twoVec = {1.4f, 1.4f, 1.4f};
	SpawnEffect("Wm_mr_cmnlandsmoke", 0, &pos, &nullRot, &twoVec);
	
	if (damage >= damageLimit * 2) {
		this->collisionCatD_Drill(apThis, apOther);
	} else {
		this->doStateChange(&StateID_Stomp);
	}
}

bool daRex_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCat7_GroundPound(apThis, apOther);
}

bool daRex_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	
	damageP = damage + 1;
	if (damage != damageP) {
		this->timer = 0;
		if (timer == 0) {this->damage++;}
	}
	
	if (timer < 10) {
		this->pos.y += 0.75;
	} else {
		this->pos.y -= 0.75;
	}
	
	S16Vec nullRot = {0,0,0};
	Vec thrVec = {2.4f, 2.4f, 2.4f};
	SpawnEffect("Wm_mr_penguinsmoke", 0, &pos, &nullRot, &thrVec);
	
	if (timer == 30) {
		if (damage >= damageLimit * 2) {
			this->doStateChange(&StateID_Die);
		} else if (damage > damageLimit) {
			this->doStateChange(&StateID_Stomp);
		} else {
			this->doStateChange(&StateID_Walk);
		}
	}
	
	this->timer++;
}

bool daRex_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daRex_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daRex_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatA_PenguinMario(apThis, apOther);
}

bool daRex_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daRex_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daRex_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}


int daRex_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("rex", "g3d/rex.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("rex");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	stillFalling = 0;
	
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;
	this->downYpos = 4.5;
	this->damageP = 0;

	damageLimit = settings >> 12 & 0xF; //nybble 9
	damage = 0;
	distance = 1;
	max_speed_inc = 1.6;
	min_speed_inc = 0.4;
	
	/* if (((daRex_c*)apThis->owner)->pos.x > this->pos.x) {direction = 0;} //going left first
	else {this->direction = 1;} //going right first */
	this->direction = 1;
	
	collMgr.calculateBelowCollisionWithSmokeEffect();
	
	ActivePhysics::Info RexPhysics; 
	RexPhysics.xDistToCenter = 0.0; 
	RexPhysics.yDistToCenter = 24.0; 
	RexPhysics.xDistToEdge = 8.0; 
	RexPhysics.yDistToEdge = 24.0; 
	RexPhysics.category1 = 0x3; 
	RexPhysics.category2 = 0x0; 
	RexPhysics.bitfield1 = 0x4F; 
	RexPhysics.bitfield2 = 0xFFFFFFFF; 
	RexPhysics.unkShort1C = 0; 
	RexPhysics.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &RexPhysics); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);

	doStateChange(&StateID_Walk);



	this->onExecute();

	return true;
}

int daRex_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();
	
	return true;
}

int daRex_c::onDelete() {
	return true;
}

int daRex_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

/////////////////////////
////  Walk State ////////
/////////////////////////

void daRex_c::beginState_Walk() {
	
	this->timer = 0;
	this->min_speed_inc = (direction) ? 0.4 : 0.5;
	this->max_speed_inc = (direction) ? 1.8 : 1.7;
	
}
void daRex_c::executeState_Walk() {
	animationChr.setUpdateRate(1.0f);
	
	this->pos.x += (direction) ? -min_speed_inc : min_speed_inc;
	
	this->rot.y = (direction) ? 0xD800 : 0x2800;
	
	if (distance) {
		if (collMgr.isOnTopOfTile()) {
			stillFalling = false;

			if (!willWalkOntoSuitableGround()) {
				pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
				if (damage > damageLimit) {doStateChange(&StateID_Turn);}
				else {doStateChange(&StateID_RealTurn);}
			}
		}
		else {
			if (!stillFalling) {
				stillFalling = true;
				pos.x = direction ? pos.x + 1.5 : pos.x - 1.5;
				if (damage > damageLimit) {doStateChange(&StateID_Turn);}
				else {doStateChange(&StateID_RealTurn);}
			}
		}
	}
	
	bool ret = calculateTileCollisions();
	if (ret) {doStateChange(&StateID_Turn);}

	if(this->animationChr.isAnimationDone()) {this->animationChr.setCurrentFrame(0.0);}
	this->timer++;
}
void daRex_c::endState_Walk() {
}

void daRex_c::beginState_Stomp() {
	
	bindAnimChr_and_setUpdateRate("stomped", 1, 0.0, 1.0);
	PlaySound(this, SE_PLY_YOSHI_HIT_ENEMY);
	
	this->pos.x += (direction) ? -max_speed_inc : max_speed_inc;
	this->timer = 0;
	
	//Decrease hitboxes' values
	aPhysics.removeFromList();

	ActivePhysics::Info RexDownPhysics; 	
	RexDownPhysics.xDistToCenter = 0.0; 
	RexDownPhysics.yDistToCenter = 12.0; 
	RexDownPhysics.xDistToEdge = 8.0; 
	RexDownPhysics.yDistToEdge = 12.0; 
	RexDownPhysics.category1 = 0x3; 
	RexDownPhysics.category2 = 0x0; 
	RexDownPhysics.bitfield1 = 0x4F; 
	RexDownPhysics.bitfield2 = 0xFFFFFFFF; 
	RexDownPhysics.unkShort1C = 0; 
	RexDownPhysics.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.addToList();
	
}
void daRex_c::executeState_Stomp() {
	animationChr.setUpdateRate(3.0f);	
	
	this->pos.x += (direction) ? -max_speed_inc : max_speed_inc;

	if (this->timer > (this->distance * 32)) {
		doStateChange(&StateID_Turn);
	}
	
	if (this->timer % 30 == 0) {
		S16Vec nullRot = {0,0,0};
		Vec sixVec = {0.9f, 0.9f, 0.9f};
		SpawnEffect("Wm_mr_yssweat", 0, &pos, &nullRot, &sixVec);
	}

	if(this->animationChr.isAnimationDone())
		this->animationChr.setCurrentFrame(0.0);

	this->timer = this->timer + 1;
}
void daRex_c::endState_Stomp() {}

void daRex_c::beginState_Die() {}
void daRex_c::executeState_Die() {
	
	if (this->scale.z != 0.99) {
		this->timer = 0;
		this->scale.z = 0.99;
	}
	bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
	
	if (animationChr.isAnimationDone()) {
		animationChr.setCurrentFrame(0.0);
	}
	
	if (this->timer == 1) {
		S16Vec nullRot = {0,0,0};
		Vec furVec = {1.8f, 1.0f, 0.8f};
		SpawnEffect("Wm_mr_yoshifirehit", 0, &pos, &nullRot, &furVec);
	}
	
	if (this->timer == 80) {this->Delete(1);} //Could die earlier
	
	this->timer++;
	
}
void daRex_c::endState_Die() {}

void daRex_c::beginState_Turn() {
	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daRex_c::executeState_Turn() {
	
	if(this->animationChr.isAnimationDone())
		this->animationChr.setCurrentFrame(0.0);

	amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {
		this->doStateChange(&StateID_Stomp);
	}
	
}
void daRex_c::endState_Turn() {}

void daRex_c::beginState_RealTurn() {
	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daRex_c::executeState_RealTurn() {

	if(this->animationChr.isAnimationDone()) {this->animationChr.setCurrentFrame(0.0);}

	amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {this->doStateChange(&StateID_Walk);}
	
}
void daRex_c::endState_RealTurn() {}