#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <gamescene.h>

extern u32 GameTimer;

#define currentTime *(u32*)((GameTimer) + 0x4)

const char* TCarcNameList [] = {
	"plus_clock",
	NULL
};

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);
static nw4r::snd::StrmSoundHandle handle;

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);

class daTimeClock_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	
	int immaGonnaDisappear;
	int chrono;
	
	static daTimeClock_c *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);

};


void daTimeClock_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_YUKI, 1);
	TimeKeeper::instance->setTime(dGameDisplay_c::instance->timer + (((this->settings >> 20 & 0xF) + 1) * 10));
	//TimeKeeper::instance->setTime(currentTime + (((this->settings >> 20 & 0xF) + 1) * 10));
	aPhysics.removeFromList();
	// ClassWithCameraInfo *cwci = ClassWithCameraInfo::instance;
	// Vec debug = ConvertStagePositionIntoScreenPosition(&dGameDisplay_c::instance->timerBox->trans, &this->pos, ClassWithCameraInfo::instance);
	// OSReport("trans.x = %d trans.y = %d trans.z %d\n", dGameDisplay_c::instance->timerBox->trans.x, dGameDisplay_c::instance->timerBox->trans.y, dGameDisplay_c::instance->timerBox->trans.z);
	// OSReport("pos.x = %d pos.y = %d pos.z %d\n", this->pos.x, this->pos.y, this->pos.z);
	// OSReport("newtrans.x = %d newtrans.y = %d newtrans.z = %d\n", debug.x, debug.y, debug.z);
	// dGameDisplay_c::instance->timerBox->trans = ConvertStagePositionIntoScreenPosition(&dGameDisplay_c::instance->timerBox->trans, &this->pos, ClassWithCameraInfo::instance);
	this->immaGonnaDisappear++;
}

void daTimeClock_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void daTimeClock_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

bool daTimeClock_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_YUKI, 1);
	TimeKeeper::instance->setTime(dGameDisplay_c::instance->timer + (((this->settings >> 20 & 0xF) + 1) * 10));
	aPhysics.removeFromList();
	this->immaGonnaDisappear++;
	return false;
}
bool daTimeClock_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}
bool daTimeClock_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTimeClock_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, STRM_BGM_YUKI, 1);
	TimeKeeper::instance->setTime(dGameDisplay_c::instance->timer + (((this->settings >> 20 & 0xF) + 1) * 10));
	aPhysics.removeFromList();
	this->immaGonnaDisappear++;
}

bool daTimeClock_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTimeClock_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTimeClock_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daTimeClock_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat9_RollingObject(apThis, apOther);
}
bool daTimeClock_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

bool daTimeClock_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}



daTimeClock_c *daTimeClock_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daTimeClock_c));
	return new(buffer) daTimeClock_c;
}


int daTimeClock_c::onCreate() {

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource("plus_clock", "g3d/plusclock.brres"));
	if((this->settings >> 16 & 0xF) == 0) {
		bodyModel.setup(rf.GetResMdl("Clock10"), &allocator, 0x224, 1, 0);
	}
	if((this->settings >> 16 & 0xF) == 1) {
		bodyModel.setup(rf.GetResMdl("Clock50"), &allocator, 0x224, 1, 0);
	}
	SetupTextures_MapObj(&bodyModel, 0);

	allocator.unlink();

	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	HitMeBaby.xDistToEdge = 12.0;
	HitMeBaby.yDistToEdge = 15.0;
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFFFFFFF;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	this->scale.x = 0.125;
	this->scale.y = 0.125;
	this->scale.z = 0.125;

	this->pos.z = 3300.0;

	this->onExecute();
	return true;
}


int daTimeClock_c::onDelete() {
	return true;
}

int daTimeClock_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void daTimeClock_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int daTimeClock_c::onExecute() {
	updateModelMatrices();

	if(immaGonnaDisappear == 1) {
		this->rot.y -= 0x1000;
		this->chrono++;
		this->scale = (Vec){this->scale.x - 0.01f, this->scale.y - 0.01f, this->scale.z - 0.01f};
		if(this->chrono == 1) {
			S16Vec rotation = {0,0,0};
			Vec scale = {1.0, 1.0, 1.0};
			SpawnEffect("Wm_ob_starcoinget_ring", 0, &this->pos, &rotation, &scale);	
		}
		if(this->chrono < 8) {
			this->pos.y += 2;
		}
		if(this->chrono > 7 && this->chrono < 16) {
			this->pos.y -= 2;
		}
		if(this->chrono > 16) {
			this->Delete(1);
		}
	}
	else {
		this->rot.y -= 0x200;
	}
	
	newTimeCounter = dGameDisplay_c::instance->timer;
	return true;
}
