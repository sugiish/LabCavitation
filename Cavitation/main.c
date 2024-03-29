/*****************************************************************************

 MPS-SW-MAIN
 
 Version:          2.0
 
 Completion date:  February 14, 2008
 
 Copyright:        Kazuya SHIBATA and Seiichi KOSHIZUKA
 hold the copyright of this code.
 (2013-2015 Added Bubble Model for Stirred Tank Analysis by Shogo KAITO)
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <float.h>

#include "define.h"
#include "extern.h"
#include "struct.h"
#include "file.h"
#include "memory.h"
#include "copy.h"
#include "bucket.h"
#include "domain.h"
#include "distance.h"
#include "neigh.h"
#include "density.h"
#include "timer.h"
#include "other.h"
#include "gravity.h"
#include "convection.h"
#include "collision.h"
#include "pressure.h"
#include "gradient.h"
#include "viscosity.h"
#include "sort.h"
#include "object.h"
#include "forcedMotion.h"
#include "init.h"
#include "finalize.h"



#include "outflow.h"
#include "inflow.h"
#include "cavitation.h"

int
main( int argumentCount, char **argumentVector ){
	_controlfp(_MCW_EM, _MCW_EM);
	//_controlfp(0, _EM_ZERODIVIDE | _EM_UNDERFLOW | _EM_OVERFLOW | _EM_INVALID);//0除算検出

	INIT_initializeParameters( argumentCount, argumentVector );
    
    for( timer.iTimeStep= 0; timer.iTimeStep < timer.finishTimeStep;  timer.iTimeStep++ ){
		TIMER_setDtAutomatically();
        
        TIMER_putTimeForwardByDt();
        
        TIMER_displayStateOfTimeStep_atAppropriateTime();

		if (parameter.flagOfKondoAndKoshizukaModel == ON) {
			DENSITY_calculateParticleNumberDensity(particle.position);
			COPY_copy1dimDoubleArray(particle.totalNumber, particle.particleNumberDensity_prevstep, particle.particleNumberDensity_previous);
			COPY_copy1dimDoubleArray(particle.totalNumber, particle.particleNumberDensity_previous, particle.particleNumberDensity);

		}

		INFLOW_setVelocity();

        GRAVITY_calculateGravity();
        
        VISCOSITY_calculateViscosity();
        
		
		//一時的に使わないよー
		//OUTFLOW_correctShibataOutflowVelocity();

        CONVECTION_moveParticles( particle.position, particle.velocity );//err?
        
		OUTFLOW_deleteOutflowParticles();
        
        if(parameter.flagOfForcedMotionOfRigidBody == ON ){
            
            FORCEDMOTION_mainFunctionOfForcedMotionOfRigidBody( &forcedMotionOfRigidBody );
            
        }
        
		INFLOW_changeInflowParticles();

        COLLISION_calculateCollisionBetweenParticles();
        
        OTHER_checkThatParticlesAreNotAllGhost();
        
        PRESSURE_calculatePressure();//ERROR!
        
        GRADIENT_correctParticleVelocityAndPositionUsingPressureGradient();
        
        if(parameter.flagOfForcedMotionOfRigidBody == ON ){
            
            FORCEDMOTION_mainFunctionOfForcedMotionOfRigidBody( &forcedMotionOfRigidBody );
            
        }
        
        COPY_updateParticleProperty();
		PRESSURE_updateAveragePressure();
		
        if(parameter.flagOfBubbleCalculation == ON){
            
			CAVITATION_calculateBubble();
        }
        
        FILE_writeCalculationResultInFile();
        
        COLLISION_calculateCollisionBetweenParticles();
        
        
        if( YES == TIMER_checkWhetherItIsTimeToFinishProgram() ) break;
        
    }
    
    FINALIZE_finalizeProgram();
    
    return(EXIT_SUCCESS);
    
}



