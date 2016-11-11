/**
 * @file		FCU__BRAKES.C
 * @brief		Brake control subsystem for the FCU
 * @author		Lachlan Grogan
 * @copyright	rLoop Inc.
 */
/**
 * @addtogroup RLOOP
 * @{ */
/**
 * @addtogroup FCU
 * @ingroup RLOOP
 * @{ */
/**
 * @addtogroup FCU__BRAKES
 * @ingroup FCU
 * @{ */

/* Keith:
 * When the brake is fully deployed the airgap is 2.5 mm.
 * The airgap increases by tan 17 deg x mm moved by lead screw.
 * If counting revolutions, every rotation of the ball screw moves the wedge 3 mm.
 * 0.9172 mm airgap increase per revolution.
 */

#include "../fcu_core.h"

#if C_LOCALDEF__LCCM655__ENABLE_THIS_MODULE == 1U

//the structure
extern struct _strFCU sFCU;



/***************************************************************************//**
 * @brief
 * Init any brakes variables, etc.
 * 
 *
 */
void vFCU_BRAKES__Init(void)
{

	//init the state machine variables
	sFCU.eBrakeStates = BRAKE_STATE__IDLE;

	//init the limit switches
	vFCU_BRAKES_SW__Init();

	//init the MLP sensor
	vFCU_BRAKES_MLP__Init();

	//init the stepper rotate module
	vFCU_BRAKES_STEP__Init();

}


/***************************************************************************//**
 * @brief
 * Process any brakes tasks.
 * 
 */
void vFCU_BRAKES__Process(void)
{
	Luint8 u8Test;

	//process the stepper driver if its active
	vSTEPDRIVE__Process();

	switch(sFCU.eBrakeStates)
	{

		case BRAKE_STATE__IDLE:
			//idle state, wait here until we are commanded to move via a chance state.
			break;

		case BRAKE_STATE__BEGIN_MOVE:
			//begin to move the brakes, setup the movement planner
			//compare brake distances with known postion, limit switch postion and MLP position before moving.
			//alert upper layer if movement not possible due to position sensor error

			sFCU.eBrakeStates = BRAKE_STATE__MOVING;

			break;

		case BRAKE_STATE__MOVING:
			//the brakes are moving,
			//monitor their performance by getting the current pos from the move planner
			//check the MLP (it will lag)
			//process any PID algo's here too

			//check the movement planner
			sFCU.sBrakes[(Luint8)FCU_BRAKE__LEFT].sMove.s32currentPos = s32FCU_BRAKES__Get_CurrentPos(FCU_BRAKE__LEFT);
			sFCU.sBrakes[(Luint8)FCU_BRAKE__RIGHT].sMove.s32currentPos = s32FCU_BRAKES__Get_CurrentPos(FCU_BRAKE__RIGHT);

			//todo, compute the brake position in terms of Ibeam distance.


			//check to see if the curent move task is done.
			u8Test = u8STEPDRIVE__Get_TaskComplete();
			if(u8Test == 0U)
			{
				//stay in state
			}
			else
			{
				//change state
				sFCU.eBrakeStates = BRAKE_STATE__MOVE_STOPPED;
			}


			break;

		case BRAKE_STATE__MOVE_STOPPED:
			//once we have completed moving, switch to stop state

			//once stopped go back to idle state
			break;


		case BRAKE_STATE__FAULT:

			//a fault has occurred
			break;

		case BRAKE_STATE__TEST:
			//just LG test area.
			vFCU_BRAKES__Move_IBeam_Distance_Microns(500);
			break;


	}

}

//move the brakes to a distance in MM from the I Beam
//approx distances are 25mm (fully open) to 0mm (fully closed)
//some calibration will be needed here.
void vFCU_BRAKES__Move_IBeam_Distance_Microns(Luint32 u32Distance)
{

	//temp

	//Tell the movement planner to go
	vFCU_BRAKES_STEP__Move(2000, 2000);

	//change state
	sFCU.eBrakeStates = BRAKE_STATE__BEGIN_MOVE;
}








#endif //#if C_LOCALDEF__LCCM655__ENABLE_THIS_MODULE == 1U
//safetys
#ifndef C_LOCALDEF__LCCM655__ENABLE_THIS_MODULE
	#error
#endif
/** @} */
/** @} */
/** @} */

