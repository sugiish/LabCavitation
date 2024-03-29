#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "extern.h"
#include "struct.h"
#include "memory.h"
#include "timer.h"
#include "other.h"
#include "file.h"
#include "mathe.h"
#include "inflow.h"
#include "stack.h"


FILE*
FILE_openFile(char *filename, char *mode){

  FILE *fp;
  char errorMessage[256];

  if ((fp=fopen(filename,mode))==NULL) {
    sprintf(errorMessage,"File-open of \"%s\" failed\n\n",filename);
    OTHER_endProgram(errorMessage);
  }

  return(fp);

}



void
FILE_closeFile( FILE *fp, char *fileName){

  char errorMessage[256];

  if ( fp== NULL) {

    sprintf(errorMessage,"The file,\"%s\", is not able to be closed because the file pointer is Null.\n\n",fileName);
    OTHER_endProgram(errorMessage);

  }else{

    fclose( fp );

  }
  
}





void
FILE_readInputFiles( int argumentCount,char **argumentVector ){


  FILE_setNameOfInputFile( argumentCount, argumentVector );

  FILE_setNameOfOutputFile( argumentCount, argumentVector );

  FILE_openLogFile();
  
  FILE_displayArgumentOfCommandLine( argumentCount, argumentVector );

  FILE_readDataFile();

  FILE_displayReadDataFile();

  FILE_readGridFile();

  FILE_readDesignationFileForWritingPressureFile();

  /*
  FILE_countNumberOfParticlesEachType();

  FILE_displayNumberOfParticles();
  */


}




void
FILE_openLogFile( void ){

  FpForLog = FILE_openFile( parameter.nameOfLogFile, "w");

}




void
FILE_setNameOfInputFile( int argumentCount,char **argumentVector ){


  FILE_setDefaultNameOfInputFile();

  if(argumentCount <= 1) return;

  strcpy(parameter.nameOfDataFile, argumentVector[1]);

  if(argumentCount <= 2) return;  

  strcpy(parameter.nameOfGridFile, argumentVector[2]);
  
  
  if(argumentCount <= 6) return;
    
  strcpy(parameter.nameOfBubbleInputFile, argumentVector[6]);

}



void
FILE_setNameOfOutputFile( int argumentCount,char **argumentVector ){


  FILE_setDefaultNameOfOutputFile();


  if(argumentCount <= 3) return;  

  //if( parameter.flagOfDivisionOfProfFile == ON){
	strcpy(parameter.nameOfDividedProfFile, argumentVector[3]);
  //}else{
	strcpy(parameter.nameOfProfFile,        argumentVector[3]);
  //}

  if(argumentCount <= 4) return;  

  strcpy(parameter.nameOfLogFile, argumentVector[4]);

  if(argumentCount <= 5) return;

  strcpy(parameter.nameOfDesignationFileForWritingPressureFile, argumentVector[5]);
  
 
  if(argumentCount <= 7) return;
  
  //if( parameter.flagOfDivisionOfVtkFile == ON){
     strcpy(parameter.nameOfDividedVtkFile, argumentVector[7]);
  //}else{
     strcpy(parameter.nameOfVtkFile,        argumentVector[7]);
  //}    
  
    if(argumentCount <= 8) return;
    
    //if( parameter.flagOfDivisionOfTorqueFile == ON){
        //strcpy(parameter.nameOfOutputTorqueFile_divided, argumentVector[8]);
    //}else{
        //strcpy(parameter.nameOfOutputTorqueFile,        argumentVector[8]);
    //}

}




void
FILE_displayArgumentOfCommandLine( int argumentCount, char **argumentVector ){

  int iArgument;

  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog,"        Command line arguments                      \n");
  fprintf(FpForLog,"====================================================\n");

  fprintf(FpForLog,"argumentCount = %d\n", argumentCount );

  for( iArgument=0; iArgument < argumentCount; iArgument++){
    fprintf(FpForLog,"argumentVector[%d] = %s\n", iArgument, argumentVector[iArgument] );
  }

}


void
FILE_setDefaultNameOfInputFile( void ){

  strcpy( parameter.nameOfDataFile, NAME_OF_DATA_FILE);

  strcpy( parameter.nameOfGridFile, NAME_OF_GRID_FILE);

  strcpy( parameter.nameOfBubbleInputFile, NAME_OF_BUBBLE_FILE);

}





void
FILE_setDefaultNameOfOutputFile( void ){

  strcpy( parameter.nameOfProfFile, NAME_OF_PROF_FILE);

  strcpy( parameter.nameOfDividedProfFile, NAME_OF_DIVIDED_PROF_FILE);

  strcpy( parameter.nameOfLogFile,  NAME_OF_LOG_FILE);
  

  strcpy( parameter.nameOfVtkFile, NAME_OF_VTK_FILE);

  strcpy( parameter.nameOfDividedVtkFile, NAME_OF_DIVIDED_VTK_FILE);

  strcpy( parameter.nameOfOutputTorqueFile, NAME_OF_TORQUE_FILE);
    
  strcpy( parameter.nameOfOutputTorqueFile_divided, NAME_OF_DIVIDED_TORQUE_FILE);


}





void
FILE_readGridFile( void ){

  FILE *fp1;

  FILE *fp2;

  int iParticle;
  int iDim;


  fp1 = FILE_openFile(parameter.nameOfGridFile,"r");

  FILE_scanDouble( fp1,&timer.simulationTime,"timer.simulationTime");
  FILE_scanInt(    fp1,&particle.totalNumber,  "particle.totalNumber");

  if (parameter.flagOfInflowBoundaryCondition == ON)
	  particle.totalNumber += parameter.numberOfExtraGhostParticles;
  else parameter.numberOfExtraGhostParticles = 0;

  particle.totalNumber_upperLimit =  particle.totalNumber;
  fprintf( stderr, "particle.totalNumber_upperLimit = %d\n", particle.totalNumber_upperLimit );

  fprintf(FpForLog,"timer.simulationTime = %lf [sec]\n",       timer.simulationTime);  
  fprintf(FpForLog,"particle.totalNumber     = %d  [particles]\n", particle.totalNumber     );

  if(parameter.flagOfBubbleCalculation==ON){
     fp2 = FILE_openFile(parameter.nameOfBubbleInputFile,"r");
  }
   
  MEMORY_allocateMemoryForParticleStructure();
  
  for(iParticle=0; iParticle < particle.totalNumber - parameter.numberOfExtraGhostParticles; iParticle++){

    FILE_scanInt( fp1,&particle.type[iParticle],"particle.type[iParticle]");

    for(iDim = 0; iDim < 3; iDim++){
      FILE_scanDouble( fp1,&particle.position[iDim][iParticle],"particle.position[iDim][iParticle]");
    }

    for(iDim = 0; iDim < 3; iDim++){
      FILE_scanDouble( fp1,&particle.velocity[iDim][iParticle],"particle.velocity[iDim][iParticle]");
    }

    FILE_scanDouble( fp1,&particle.pressure[iParticle],"particle.pressure[iParticle]");
    FILE_scanDouble( fp1,&particle.particleNumberDensity[iParticle],"particle.particleNumberDensity[iParticle]");
      
    if(parameter.flagOfBubbleCalculation==ON){
        FILE_scanDouble( fp2,&particle.moleOfBubbles[iParticle],"particle.moleOfBubbles[iParticle]");
        FILE_scanDouble( fp2,&particle.numberOfBubbles[iParticle],"particle.numberOfBubbles[iParticle]");
        FILE_scanDouble( fp2,&particle.concentrationOfImpurities[iParticle],"particle.concentrationOfImpurities[iParticle]");
		
    }


	particle.isInflow[iParticle] = FALSE;
	particle.cavitationBubblesRadius[iParticle] = 0.0;


	if (parameter.flagOfInflowBoundaryCondition) {
		if (particle.type[iParticle] == parameter.inflowType) {
			particle.isInflow[iParticle] = TRUE;
		}
	}

  }

  if (parameter.flagOfInflowBoundaryCondition == ON) {
	  for (iParticle = particle.totalNumber - parameter.numberOfExtraGhostParticles; iParticle < particle.totalNumber; iParticle++) {
			particle.moleOfBubbles[iParticle] = 0;
			particle.numberOfBubbles[iParticle] = 0;
			particle.concentrationOfImpurities[iParticle] = 0;
			OTHER_changeParticleTypeIntoGhost(iParticle);

	  }
  }

  FILE_closeFile( fp1, parameter.nameOfGridFile );

}





void
FILE_readDesignationFileForWritingPressureFile( void ){

  FILE  *fp;
  int   iDesignatedParticle;


  if( parameter.flagOfOutputOfPressureFile == OFF )return;
  

  fp = FILE_openFile(parameter.nameOfDesignationFileForWritingPressureFile,"r");


  FILE_scanInt(  fp,  &parameter.numberOfDesignatedParticles,    "parameter.numberOfDesignatedParticles");

  fprintf(FpForLog,"parameter.numberOfDesignatedParticles  = %d  [particles]\n", parameter.numberOfDesignatedParticles);


  particle.listOfDesignatedParticles = MEMORY_allocateMemoryFor1dimIntArray( 
                                                                       parameter.numberOfDesignatedParticles
                                                                     ,"particle.listOfDesignatedParticles" );


  for(iDesignatedParticle=0; iDesignatedParticle < parameter.numberOfDesignatedParticles; iDesignatedParticle++){

    FILE_scanInt( fp,&particle.listOfDesignatedParticles[iDesignatedParticle]
		      ,"particle.listOfDesignatedParticles[iDesignatedParticle]");
  }


  FILE_closeFile( fp, parameter.nameOfDesignationFileForWritingPressureFile );

}





void
FILE_countNumberOfParticlesEachType( void ){

  int iParticle;

  parameter.numberOfFluidParticles     = 0;  
  parameter.numberOfRigidParticles     = 0;  
  parameter.numberOfWallParticles      = 0;
  parameter.numberOfDummyWallParticles = 0;
  parameter.numberOfGhostParticles     = 0;


  for(iParticle=0; iParticle < particle.totalNumber; iParticle++){  

    if( particle.type[iParticle] == parameter.wallType ){

      parameter.numberOfWallParticles++;

    }else if( particle.type[iParticle] == parameter.dummyWallType ){

      parameter.numberOfDummyWallParticles++;

    }else if( particle.type[iParticle] == parameter.typeNumberOfRigidParticle_forForcedMotion ){

      parameter.numberOfRigidParticles++;

    }else if( particle.type[iParticle] == GHOST ){

      parameter.numberOfGhostParticles++;

    }else{

      parameter.numberOfFluidParticles++;

    }

  }

}



void
FILE_displayNumberOfParticles( void ){

  fprintf(FpForLog,"\n");
  fprintf(FpForLog,"\n");
  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog,"        the number of particles                     \n");
  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog, "total                   = %d\n",particle.totalNumber                 );
  fprintf(FpForLog, "  ---fluidParticles     = %d\n",parameter.numberOfFluidParticles     );
  fprintf(FpForLog, "  ---rigidParticles     = %d\n",parameter.numberOfRigidParticles     );
  fprintf(FpForLog, "  ---wallParticles      = %d\n",parameter.numberOfWallParticles      );
  fprintf(FpForLog, "  ---dummyWallParticles = %d\n",parameter.numberOfDummyWallParticles );
  fprintf(FpForLog,"\n");
  fprintf(FpForLog,"\n");
  fprintf(FpForLog,"\n");

}



void
FILE_scanDouble( FILE *fp, double *scanedValue, char *parameterName){

  int status;
  status = fscanf(fp, "%lf", scanedValue);
  FILE_checkEndOfFile(status, parameterName);

}



void
FILE_scanInt( FILE *fp, int *scanedValue, char *parameterName){

  int status;
  status = fscanf(fp, "%d", scanedValue);
  FILE_checkEndOfFile(status, parameterName);

}




void
FILE_scanChar( FILE *fp, char *scanedValue, char *parameterName){

  int status;
  status = fscanf(fp, "%s", scanedValue);
  FILE_checkEndOfFile(status, parameterName);

}



void
FILE_scanOnOff(FILE *fp, int *flag, char *variable_name){
  char buf[256];
  char errorMessage[256];

  fscanf(fp,"%s", buf);   
  if( ((strcmp( buf, "ON" ) == 0 )
       ||(strcmp( buf, "on" ) == 0 ))
       ||(strcmp( buf, "On" ) == 0 )){

    (*flag) = ON;

  }else if( ((strcmp( buf, "OFF" ) == 0 )
	     || (strcmp( buf, "off" ) == 0 ))
	     || (strcmp( buf, "Off" ) == 0 )){

    (*flag) = OFF;

  }else{
    sprintf(errorMessage,"Error: Parameter of \"%s\" is not adequate. [ in data-file]\n", variable_name);
	sprintf(errorMessage, "%s\n", buf);
	
	OTHER_endProgram(errorMessage);
  }

}





void
FILE_scanTypeOfOutputInterval(FILE *fp, int *flag, char *variable_name){
  char buf[256];
  char errorMessage[256];

  fscanf(fp,"%s", buf);   
  if( ((strcmp( buf, "simulationTime" ) == 0 )
       ||(strcmp( buf, "SimulationTime" ) == 0 ))
       ||(strcmp( buf, "SIMULATION_TIME" ) == 0 )){

    (*flag) = SIMULATION_TIME;


  }else if( ((strcmp( buf, "timeStep" ) == 0 )
	     || (strcmp( buf, "TimeStep" ) == 0 ))
	     || (strcmp( buf, "TIME_STEP" ) == 0 )){


    (*flag) = TIME_STEP;


  }else{
    sprintf(errorMessage,"Error: Parameter of \"%s\" is not adequate. [ in data-file]\n", variable_name);
    OTHER_endProgram(errorMessage);
  }

}



void
FILE_scanMotionTypeOfRigidBody(FILE *fp, int *flag, char *variable_name){
  char buf[256];
  char errorMessage[256];

  fscanf(fp,"%s", buf);   
  if( (((strcmp( buf, "forcedMotion" ) == 0 )
	||(strcmp( buf, "forcedmotion" ) == 0 )))
        ||((strcmp( buf, "ForcedMotion" ) == 0 )
        ||(strcmp( buf, "FORCED_MOTION" ) == 0 ))){

    (*flag) = FORCED_MOTION;

  }else if( (((strcmp( buf, "compulsiveMotion" ) == 0 )
	||(strcmp( buf, "compulsivemotion" ) == 0 )))
        ||((strcmp( buf, "CompulsiveMotion" ) == 0 )
        ||(strcmp( buf, "COMPULSIVE_MOTION" ) == 0 ))){

    (*flag) = FORCED_MOTION;

  }else{
    sprintf(errorMessage,"Error: Parameter of \"%s\" is not adequate. [ in data-file]\n", variable_name);
    OTHER_endProgram(errorMessage);
  }

}





char*
FILE_returnOnOff(int flag){

  if(flag == ON){

    return("on");

  }else if(flag == OFF){

    return("off");

  }else{

    return("***--ERROR--****");

  }

}





char*
FILE_returnDim(int iDim){

  if(iDim == XDIM){
    return("XDIM");
  }else if(iDim == YDIM){
    return("YDIM");
  }else if(iDim == ZDIM){
    return("ZDIM");
  }

  return("##---ERROR---##");

}



char*
FILE_returnTypeOfOutputInterval(int flag){

  if(flag == SIMULATION_TIME){

    return("simulationTime");

  }else if(flag == TIME_STEP){

    return("timeStep");

  }else{

    return("***--ERROR--****");

  }

}





char*
FILE_returnMotionTypeOfRigidBody(int flag){

  if(flag == FORCED_MOTION){

    return("forcedMotion");

  }else{

    return("***--ERROR--****");

  }

}



void
FILE_skipComment( FILE *fp ){

  char comment[256];

  fscanf(fp,"%s",comment);  

}




void
FILE_checkEndOfFile(int status, char *name){

  if(status == EOF){
    fprintf(FpForLog,"ERROR: Scan of '%s' failed.\n", name);
    fprintf(FpForLog,"\n");
    exit(1);
  }

}


void
FILE_displayGridInformation( void ){

  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog,"        grid information                            \n");
  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog,"physical time       =  %lf\n", timer.simulationTime);
  fprintf(FpForLog,"number of particles =  %d\n",  particle.totalNumber     );
  fprintf(FpForLog,"\n\n");

}



void
FILE_writeCalculationResultInFile( void ){


  if( YES == TIMER_checkWhetherItIsTimeToWriteProfFile()){

    if(timer.flagOfDisplayingStateOfTimeStep == OFF){
      TIMER_displayStateOfTimeStep( stderr );
    }

    FILE_writeProfFile();

  }else if(timer.flagOfDisplayingStateOfTimeStep == ON){
    fprintf(stderr," \n");
  }


  FILE_writePressureFile();

}





void
FILE_writeProfFile( void ){

  FILE *fp;
  int   iParticle;
  int   iDim;
  char  fileName[256];


  fp = FILE_openProfFile();

  fprintf(fp, "%lf\n", timer.simulationTime);
  fprintf(fp, "%d\n",  particle.totalNumber);
  

  if( parameter.flagOfExponentialExpressionInProfFile == ON ){

	for(iParticle=0; iParticle < particle.totalNumber; iParticle++){

	  fprintf(fp,"%d",particle.type[iParticle]);

	  for(iDim = 0; iDim < 3; iDim++){
		fprintf(fp," %e",particle.position[iDim][iParticle]);
	  }

	  for(iDim = 0; iDim < 3; iDim++){
		fprintf(fp," %e",particle.velocity[iDim][iParticle]);
	  }

	  fprintf(fp," %e",particle.pressure[iParticle]);
	  fprintf(fp," %e",particle.particleNumberDensity[iParticle]);

      if( parameter.flagOfBubbleCalculation == ON){
          fprintf(fp," %e",particle.moleOfBubbles[iParticle]);
          fprintf(fp," %e",particle.numberOfBubbles[iParticle]);
		  fprintf(fp, " %e", particle.cavitationBubblesRadius[iParticle]);
		  //fprintf(fp," %e",particle.concentrationOfImpurities[iParticle]);
      }

        
	  fprintf(fp,"\n");
	}


  }else{

	for(iParticle=0; iParticle < particle.totalNumber; iParticle++){
	  fprintf(fp,"%d",particle.type[iParticle]);

	  for(iDim = 0; iDim < 3; iDim++){
		fprintf(fp," %lf",particle.position[iDim][iParticle]);
	  }

	  for(iDim = 0; iDim < 3; iDim++){
		fprintf(fp," %lf",particle.velocity[iDim][iParticle]);
	  }

	  fprintf(fp," %lf",particle.pressure[iParticle]);
	  fprintf(fp," %lf",particle.particleNumberDensity[iParticle]);

      if( parameter.flagOfBubbleCalculation == ON){
            fprintf(fp," %e",particle.moleOfBubbles[iParticle]);
            fprintf(fp," %e",particle.numberOfBubbles[iParticle]);
			fprintf(fp, " %e", particle.cavitationBubblesRadius[iParticle]);
			//  fprintf(fp," %e",particle.concentrationOfImpurities[iParticle]);
      }

	  fprintf(fp,"\n");
	}

  }



  fflush(fp);
  FILE_closeFile(fp, fileName );

  fprintf(stderr,   "%d-th profFile was written.\n", timer.iProfFile);
  fprintf(FpForLog, "%d-th profFile was written.\n", timer.iProfFile);

  FILE_compressProfFile();

  FILE_makeVtkFile();


  timer.iProfFile++;

}


void
FILE_makeVtkFile( void ){
    
    FILE *fp;
    int   iParticle;
    int   iDim;
    char filename[256];
    
    double diameter;
    double Force[3];
    double Advection[3];
    double Torque[3];
    double Y_Torque;
    
    fp=FILE_openVtkFile();
    
    //sprintf(filename, "output_%04d.vtk",particle.totalNumber);
    //printf("Creating %s ... ", filename);
    
    //fp=fopen(filename,"w");
    fprintf(fp,"# vtk DataFile Version 3.0\n");
    fprintf(fp,"vtk output\n");
    fprintf(fp,"ASCII\n");
    fprintf(fp,"DATASET UNSTRUCTURED_GRID\n");
    
    fprintf(fp,"POINTS %d float\n",particle.totalNumber);
    for(iParticle=0; iParticle < particle.totalNumber; iParticle++){
        for(iDim = 0; iDim < 3; iDim++){
            fprintf(fp,"%lf ",particle.position[iDim][iParticle]);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"\n");
    
    fprintf(fp,"CELLS %d %d\n",particle.totalNumber,particle.totalNumber*2);
    for(iParticle=0; iParticle < particle.totalNumber; iParticle++){
        fprintf(fp,"1 %d\n",iParticle);
    }
    fprintf(fp,"\n");
    
    fprintf(fp,"CELL_TYPES %d\n",particle.totalNumber);
    for(iParticle=0; iParticle < particle.totalNumber; iParticle++){
        fprintf(fp,"1\n");
    }
    fprintf(fp,"\n");
    
    fprintf(fp,"POINT_DATA %d\n",particle.totalNumber);
    
    fprintf(fp,"SCALARS type int %d\n",1);
    fprintf(fp,"LOOKUP_TABLE type\n");
    for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
        fprintf(fp,"%d\n",particle.type[iParticle]);
    }
    fprintf(fp,"\n");
    
    fprintf(fp,"SCALARS Velocity float 1\n");
    fprintf(fp,"LOOKUP_TABLE Velocity\n");
    for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
        double val=sqrt(particle.velocity[0][iParticle]*particle.velocity[0][iParticle]+particle.velocity[1][iParticle]*particle.velocity[1][iParticle]+particle.velocity[2][iParticle]*particle.velocity[2][iParticle]);
        fprintf(fp,"%lf\n",val);
    }
    fprintf(fp,"\n");
    
    fprintf(fp,"SCALARS Pressure float 1\n");
    fprintf(fp,"LOOKUP_TABLE Pressure\n");
    for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
        fprintf(fp,"%lf\n",particle.pressure[iParticle]);
    }
    fprintf(fp,"\n");
    
    fprintf(fp,"SCALARS particleNumberDensity float 1\n");
    fprintf(fp,"LOOKUP_TABLE particleNumberDensity\n");
    for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
        fprintf(fp,"%lf\n",particle.particleNumberDensity[iParticle]);
    }
    fprintf(fp,"\n");

	fprintf(fp, "SCALARS boundaryCondition int 1\n");
	fprintf(fp, "LOOKUP_TABLE boundaryCondition\n");
	for (iParticle = 0; iParticle<particle.totalNumber; iParticle++) {
		fprintf(fp, "%d\n", particle.flagOfBoundaryCondition[iParticle]);
	}
	fprintf(fp, "\n");

    
    if(parameter.flagOfBubbleCalculation==ON){
    
       fprintf(fp,"SCALARS moleOfBubbles double 1\n");
       fprintf(fp,"LOOKUP_TABLE moleOfBubbles\n");
       for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
           fprintf(fp,"%le\n",particle.moleOfBubbles[iParticle]);
       }
       fprintf(fp,"\n");
    
       fprintf(fp,"SCALARS numberOfBubbles double 1\n");
       fprintf(fp,"LOOKUP_TABLE numberOfBubbles\n");
       for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
           fprintf(fp,"%le\n",particle.numberOfBubbles[iParticle]);
       }
       fprintf(fp,"\n");
    
       fprintf(fp,"SCALARS RatioOfImpurities double 1\n");
       fprintf(fp,"LOOKUP_TABLE RatioOfImpurities\n");
       for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
           fprintf(fp,"%le\n", particle.concentrationOfImpurities[iParticle]);
       }
       fprintf(fp,"\n");
    
    
       fprintf(fp,"SCALARS BubbleDiameter float 1\n");
       fprintf(fp,"LOOKUP_TABLE BubbleDiameter\n");
       for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
           //diameter=Bubble_calculateDiameter(iParticle);
		   diameter = particle.cavitationBubblesRadius[iParticle];
		   fprintf(fp,"%lf\n", diameter);
       }
       fprintf(fp,"\n");

	   fprintf(fp, "SCALARS voidrateOfParticle double 1\n");
	   fprintf(fp, "LOOKUP_TABLE voidrateOfParticle\n");
	   for (iParticle = 0; iParticle<particle.totalNumber; iParticle++) {
		   fprintf(fp, "%lf\n", particle.voidrateOfParticle[iParticle]);
	   }
	   fprintf(fp, "\n");

    }
    
    fprintf(fp,"VECTORS point_vectors float\n");
	for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
		fprintf(fp,"%lf %lf %lf\n",particle.velocity[XDIM][iParticle],particle.velocity[YDIM][iParticle],particle.velocity[ZDIM][iParticle]);
	}
    fprintf(fp,"\n");


	fprintf(fp, "VECTORS velocity_correction float\n");
	for (iParticle = 0; iParticle<particle.totalNumber; iParticle++) {
		fprintf(fp, "%lf %lf %lf\n", particle.velocity_correction[XDIM][iParticle], particle.velocity_correction[YDIM][iParticle], particle.velocity_correction[ZDIM][iParticle]);
	}
	fprintf(fp, "\n");

	fprintf(fp, "VECTORS velocity_vector float\n");
	for (iParticle = 0; iParticle < particle.totalNumber; iParticle++) {
		fprintf(fp, "%lf %lf %lf\n", particle.velocity[XDIM][iParticle], particle.velocity[YDIM][iParticle], particle.velocity[ZDIM][iParticle]);
	}
	fprintf(fp, "\n");

	fprintf(fp, "SCALARS SourceTerm float 1\n");
	fprintf(fp, "LOOKUP_TABLE SourceTerm\n");
	for (iParticle = 0; iParticle<particle.totalNumber; iParticle++) {
		fprintf(fp, "%lf\n", particle.sourceTermOfPressure[iParticle]);
	}
	fprintf(fp, "\n");


    
    if(parameter.flagOfOutputOfTorqueFile == ON ){
       fprintf(fp,"VECTORS Torque float\n");
        
       Y_Torque=0.0;
	   for(iParticle=0;iParticle<particle.totalNumber;iParticle++){
         
        if(timer.iVtkFile == 0){
            Torque[XDIM]=0.0;
            Torque[YDIM]=0.0;
            Torque[ZDIM]=0.0;
            Y_Torque += 2.0*M_PI*1.0*Torque[YDIM];
            fprintf(fp,"%lf %lf %lf\n",Torque[XDIM],Torque[YDIM],Torque[ZDIM]);
        }else{
           
           if(particle.type[iParticle]==parameter.typeNumberOfRigidParticle_forForcedMotion){
        
              for(iDim=0;iDim< NumberOfDimensions;iDim++){
                  Force[iDim]=physicalProperty.massDensity[particle.type[iParticle]]*(4.0/3.0)*M_PI*pow((particle.averageDistance/2.0),3.0)*(1.0/timer.dt)*(particle.velocity3[iDim][iParticle]-particle.velocity2[iDim][iParticle]);
              }
           }else{
              for(iDim=0;iDim< NumberOfDimensions;iDim++){
                   Force[iDim]=0.0;
              }
           }
           Advection[XDIM]=particle.position2[XDIM][iParticle];
           Advection[YDIM]=0.0;
           Advection[ZDIM]=particle.position2[ZDIM][iParticle];
           
           MATHE_outerProduct(Torque,Advection,Force);
           Y_Torque += 2.0*M_PI*1.0*Torque[YDIM];
           
           fprintf(fp,"%lf %lf %lf\n",Torque[XDIM],Torque[YDIM],Torque[ZDIM]);
        }
       }
       printf("Torque[Pressure]=%lf\n",Y_Torque);
       fprintf(fp,"\n");
    }

	if (parameter.flagOfAveragePressureInEachBucket) {
		fprintf(fp, "SCALARS AveragePressureInEachBucket float 1\n");
		fprintf(fp, "LOOKUP_TABLE AveragePressureInEachBucket\n");
		for (iParticle = 0; iParticle<particle.totalNumber; iParticle++) {
			fprintf(fp, "%lf\n", particle.bucketPressure[iParticle]);
		}
		fprintf(fp, "\n");

	}
    
    printf("done.\n");
    
    fflush(fp);
    FILE_closeFile(fp, filename);
    
    timer.iVtkFile++;
}




void
FILE_writePressureFile( void ){

  if( parameter.flagOfOutputOfPressureFile == OFF ) return;

  if( parameter.flagOfOutputOfAllWallParticle == ON ){

    FILE_outputPressureOfAllWallParticles();

  }else{

    FILE_outputPressureOfOnlyDesignatedParticles();

  }

}



void
FILE_outputPressureOfAllWallParticles( void ){

  FILE *fp;
  int  iParticle;
  int  iDim;

  fp = FILE_openPressureFile();

  fprintf(fp, "%lf\n", timer.simulationTime             );
  fprintf(fp, "%d\n",  parameter.numberOfWallParticles );


  for(iParticle=0; iParticle < particle.totalNumber; iParticle++){

    if( particle.type[iParticle] == parameter.wallType ){

      fprintf(fp,"%d",iParticle);
      fprintf(fp,"  %d",particle.type[iParticle]);

      for(iDim = 0; iDim < 3; iDim++){
		fprintf(fp," %lf", particle.position[iDim][iParticle]);
      }

      for(iDim = 0; iDim < 3; iDim++){
		fprintf(fp," %lf", particle.velocity[iDim][iParticle]);
      }

      fprintf(fp," %lf", particle.pressure[iParticle]);
      fprintf(fp," %lf", particle.particleNumberDensity[iParticle]);
      fprintf(fp,"\n");

    }

  }


  fflush(fp);
  FILE_closeFile( fp, "output.press" );

  /*
  fprintf(stderr,   "%d-th pressureFile was written.\n", timer.iPressureFile);
  */
  fprintf(FpForLog, "%d-th pressureFile was written.\n", timer.iPressureFile);

  timer.iPressureFile++;

}





void
FILE_outputPressureOfOnlyDesignatedParticles( void ){

  FILE  *fp;
  int   iParticle;
  int   iDesignatedParticle;
  int   iDim;
  char  fileName[256];


  fp = FILE_openPressureFile();

  fprintf(fp, "%lf\n", timer.simulationTime);
  fprintf(fp, "%d\n",  parameter.numberOfDesignatedParticles   );

  
  for(iDesignatedParticle=0; iDesignatedParticle < parameter.numberOfDesignatedParticles; iDesignatedParticle++){

    iParticle = particle.listOfDesignatedParticles[iDesignatedParticle];

    fprintf(fp,"%d  %d",iParticle, particle.type[iParticle]);

    for(iDim = 0; iDim < 3; iDim++){
      fprintf(fp," %lf",particle.position[iDim][iParticle]);
    }

    for(iDim = 0; iDim < 3; iDim++){
      fprintf(fp," %lf",particle.velocity[iDim][iParticle]);
    }

    fprintf(fp," %lf",particle.pressure[iParticle]);
    fprintf(fp," %lf",particle.particleNumberDensity[iParticle]);

    fprintf(fp,"\n");
  }

  fflush(fp);
  FILE_closeFile(fp, fileName );


  fprintf(FpForLog, "%d-th pressureFile was written.\n", timer.iPressureFile);
  /*
  fprintf(stderr, "%d-th pressureFile was written.\n", timer.iPressureFile);
  */

  timer.iPressureFile++;

}


FILE*
FILE_openProfFile( void ){

  FILE *fp;
  char  fileName[256];


  if( parameter.flagOfDivisionOfProfFile == ON){

     sprintf(fileName,"%s%04d.prof", parameter.nameOfDividedProfFile, timer.iProfFile);
     fp = FILE_openFile(fileName,"w");

  }else{

     sprintf(fileName,"%s", parameter.nameOfProfFile );

     if( timer.iProfFile == timer.initialSequantialNumberOfProfFile ){

       fp = FILE_openFile(fileName,"w");

     }else{

       fp = FILE_openFile(fileName,"a");

     }

  }

  return fp;

}

FILE*
FILE_openVtkFile( void ){
    
    FILE *fp;
    char  fileName[256];
    
    
    if( parameter.flagOfDivisionOfVtkFile == ON){
        
        sprintf(fileName,"%s%04d.vtk", parameter.nameOfDividedVtkFile, timer.iProfFile);
        fp = FILE_openFile(fileName,"w");
        
    }else{
        
        sprintf(fileName,"%s", parameter.nameOfVtkFile );
        
        if( timer.iProfFile == timer.initialSequantialNumberOfVtkFile ){
            
            fp = FILE_openFile(fileName,"w");
            
        }else{
            
            fp = FILE_openFile(fileName,"a");
            
        }
        
    }
    
    return fp;
    
}



FILE*
FILE_openPressureFile( void ){

  FILE *fp;
  char  fileName[256];

  if( parameter.flagOfDivisionOfPressureFile == ON){

     sprintf(fileName,"%s%05d.pressure", parameter.nameOfOutputPressureFile_divided, timer.iPressureFile);
     fp = FILE_openFile(fileName,"w");

  }else{

     sprintf(fileName,"%s", parameter.nameOfOutputPressureFile );

     if( timer.iProfFile == 0 ){

       fp = FILE_openFile(fileName,"w");

     }else{

       fp = FILE_openFile(fileName,"a");

     }
  }

  return fp;

}



void
FILE_compressProfFile( void ){


  char  fileName[SIZE_OF_FILE_NAME];
  char  command[SIZE_OF_FILE_NAME];

  if( parameter.flagOfCompressionOfProfFile != ON) return;

  if( parameter.flagOfDivisionOfProfFile == ON){

     sprintf(fileName,"%s%04d.prof", parameter.nameOfDividedProfFile, timer.iProfFile);
     sprintf(command,"gzip -9 %s",fileName);
     system(command);

     fprintf(stderr,   "%d-th profFile was compressed.\n", timer.iProfFile);
     fprintf(FpForLog, "%d-th profFile was compressed.\n", timer.iProfFile);

  }

}



void
FILE_readDataFile( void ){

  int  iType;
  int  iDim;
  char variableName[256];
  FILE *fp;


  fp = FILE_openFile(parameter.nameOfDataFile,"r");

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanDouble( fp,&particle.averageDistance
                     ,"particle.averageDistance");

  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanDouble( fp,&timer.finishTime
                     ,"timer.finishTime");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&timer.dt_initial
                     ,"timer.dt_initial");

  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanTypeOfOutputInterval( fp,&timer.typeOfOutputInterval
                                  ,"typeOfOutputInterval");


  FILE_skipComment(fp);
  FILE_scanDouble( fp,&timer.intervalTimeOfWritingProfFile_simulationTime
                     ,"timer.intervalTimeOfWritingProfFile_simulationTime");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&timer.intervalTimeOfWritingProfFile_timeStep
                  ,"timer.intervalTimeOfWritingProfFile_timeStep");

  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanInt( fp,&NumberOfDimensions
                   ,"NumberOfDimensions");

  FILE_skipComment(fp);


  FILE_scanOnOff(  fp,&domain.flagOfAutoSettingOfDomainSize
                     ,"domain.flagOfAutoSettingOfDomainSize");


  for(iDim=0; iDim < 3; iDim++){
    FILE_skipComment(fp);
    sprintf(variableName,"domain.upperMarginRatio[%d]",iDim);
    FILE_scanDouble( fp,&domain.upperMarginRatio[iDim],variableName);
  }

  for(iDim=0; iDim < 3; iDim++){
    FILE_skipComment(fp);
    sprintf(variableName,"domain.lowerMarginRatio[%d]",iDim);
    FILE_scanDouble( fp,&domain.lowerMarginRatio[iDim],variableName);
  }


  for(iDim=0; iDim < 3; iDim++){
    FILE_skipComment(fp);
    sprintf(variableName,"domain.upperLimit[%d]",iDim);
    FILE_scanDouble( fp,&domain.upperLimit[iDim],variableName);
  }

  for(iDim=0; iDim < 3; iDim++){
    FILE_skipComment(fp);
    sprintf(variableName,"domain.lowerLimit[%d]",iDim);
    FILE_scanDouble( fp,&domain.lowerLimit[iDim],variableName);
  }


  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.radiusOfParticleNumberDensity_ratio
                     ,"parameter.radiusOfParticleNumberDensity_ratio");


  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.radiusOfGradient_ratio
                     ,"parameter.radiusOfGradient_ratio");


  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.radiusOfLaplacianForViscosity_ratio
                     ,"parameter.radiusOfLaplacianForViscosity_ratio");


  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.radiusOfLaplacianForPressure_ratio
                     ,"parameter.radiusOfLaplacianForPressure_ratio");

  FILE_skipComment(fp);
  FILE_skipComment(fp);


  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanOnOff(  fp,&domain.flagOfAutoSettingOfBucketCapacity
                     ,"domain.flagOfAutoSettingOfBucketCapacity");

  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&domain.flagOfOptimizationOfBucketMemorySize
                     ,"domain.flagOfOptimizationOfBucketMemorySize");


  FILE_skipComment(fp);
  FILE_scanDouble( fp,&domain.marginRatioForSettingBucketCapacity
                     ,"domain.marginRatioForSettingBucketCapacity");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&domain.capacityOfBucket
                  ,"domain.capacityOfBucket");


  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.numberOfNeighborTables
                  ,"parameter.numberOfNeighborTables");


  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&parameter.flagOfAutoSettingOfCapacityOfNeighborTable
                     ,"parameter.flagOfAutoSettingOfCapacityOfNeighborTable");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.marginRatioForSettingCapacityOfLargeNeighborTable
                     ,"parameter.marginRatioForSettingCapacityOfLargeNeighborTable");


  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.marginRatioForSettingCapacityOfSmallNeighborTable
                     ,"parameter.marginRatioForSettingCapacityOfSmallNeighborTable");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.capacityOfNeighborTable_large
                  ,"parameter.capacityOfNeighborTable_large");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.capacityOfNeighborTable_small
                  ,"parameter.capacityOfNeighborTable_small");


  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanOnOff(  fp,&parameter.flagOfDivisionOfProfFile
                     ,"parameter.flagOfDivisionOfProfFile");

  FILE_skipComment(fp);
  FILE_scanOnOff( fp,&parameter.flagOfCompressionOfProfFile
                  ,"parameter.flagOfCompressionOfProfFile");


  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.upperLimitOfNumberOfProfFiles
                  ,"parameter.upperLimitOfNumberOfProfFiles");


  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanOnOff(  fp,&parameter.flagOfOutputOfPressureFile
                     ,"parameter.flagOfOutputOfPressureFile");

  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&parameter.flagOfOutputOfAllWallParticle
                     ,"parameter.flagOfOutputOfAllWallParticle");

  FILE_skipComment(fp);
  FILE_scanChar( fp, parameter.nameOfDesignationFileForWritingPressureFile
                   ,"parameter.nameOfDesignationFileForWritingPressureFile");

  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&parameter.flagOfDivisionOfPressureFile
                     ,"parameter.flagOfDivisionOfPressureFile");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.upperLimitOfNumberOfPressureFiles
                  ,"parameter.upperLimitOfNumberOfPressureFiles");


  FILE_skipComment(fp);
  FILE_scanChar( fp, parameter.nameOfOutputPressureFile_divided
                   ,"parameter.nameOfOutputPressureFile_divided");

  FILE_skipComment(fp);
  FILE_scanChar( fp, parameter.nameOfOutputPressureFile
                   ,"parameter.nameOfOutputPressureFile");
  
  FILE_skipComment(fp);
  FILE_skipComment(fp);
    
  FILE_scanOnOff(  fp,&parameter.flagOfOutputOfTorqueFile
                   ,"parameter.flagOfOutputOfTorqueFile");
    
  FILE_skipComment(fp);
  FILE_scanChar( fp, parameter.nameOfOutputTorqueFile
                  ,"parameter.nameOfOutputTorqueFile");
  
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.numberOfRotations
                    ,"parameter.numberOfRotations");


  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.numberOfParticleTypes
                  ,"parameter.numberOfParticleTypes");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.wallType
                  ,"parameter.wallType");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.dummyWallType
                  ,"parameter.dummyWallType");


  physicalProperty.massDensity      = MEMORY_allocateMemoryFor1dimDoubleArray( parameter.numberOfParticleTypes, "massDenisity"   );

  physicalProperty.compressibility  = MEMORY_allocateMemoryFor1dimDoubleArray( parameter.numberOfParticleTypes, "compressibility");


  FILE_skipComment(fp);
  for(iType=0; iType < parameter.numberOfParticleTypes; iType++){
    FILE_skipComment(fp);
    FILE_scanDouble( fp,&physicalProperty.massDensity[iType]
                       ,"physicalProperty.massDensity[iType]");
  }


  FILE_skipComment(fp);
  for(iType=0; iType < parameter.numberOfParticleTypes; iType++){
    FILE_skipComment(fp);
    FILE_scanDouble( fp,&physicalProperty.compressibility[iType]
                       ,"physicalProperty.compressibility[iType]");
  }

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&parameter.flagOfViscosityCalculation
                     ,"parameter.flagOfViscosityCalculation");
  
  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&parameter.flagOfHighViscosityCalculation
                   ,"parameter.flagOfHighViscosityCalculation");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&physicalProperty.kinematicViscosity
                     ,"physicalProperty.kinematicViscosity");
  
  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanOnOff(  fp,&parameter.flagOfBubbleCalculation
                   ,"parameter.flagOfBubbleCalculation");
    
  FILE_skipComment(fp);
  FILE_scanChar( fp, parameter.nameOfBubbleInputFile
                  ,"parameter.nameOfBubbleInputFile");
  
  FILE_skipComment(fp);
  FILE_scanInt( fp, &parameter.numperOfParticleForCalculatingBeta
                  ,"parameter.numperOfParticleForCalculatingBeta");
    
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&physicalProperty.massDensityOfBubble
                    ,"physicalProperty.massDensityOfBubble");
    
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&physicalProperty.gasConstant
                    ,"physicalProperty.gasConstant");
  
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&physicalProperty.temperature
                    ,"physicalProperty.temperature");
  
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&physicalProperty.headPressure
                    ,"physicalProperty.headPressure");
  
  FILE_skipComment(fp);//outflowboundaryCondition

  FILE_skipComment(fp);
  FILE_scanOnOff(fp, &parameter.flagOfOutflowBoundaryCondition , "parameter.flagOfOutflowBoundaryCondition");
  
  FILE_skipComment(fp);
  FILE_scanOnOff(fp, &parameter.flagOfNegativePressure, "parameter.flagOfNegativePressure");

  for (iDim = 0; iDim < 3; iDim++) {
	  FILE_skipComment(fp);
	  sprintf(variableName, "parameter.positionVector[%d]", iDim);
	  FILE_scanDouble(fp, &parameter.positionVector[iDim], variableName);
  }
  
  for (iDim = 0; iDim < 3; iDim++) {
	  FILE_skipComment(fp);
	  sprintf(variableName, "parameter.normalVector[%d]", iDim);
	  FILE_scanDouble(fp, &parameter.normalVector[iDim], variableName);
  }
  
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.lengthOfFixedVelocityRegion, "parameter.lengthOfFixedVelocityRegion");
  
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.lengthOfOutflow, "parameter.lengthOfOutflow");

  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.widthOfOutflow, "parameter.widthOfOutflow");

  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.depthOfOutflow, "parameter.depthOfOutflow");
  
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.relaxationCoefficientOfBoundaryCondition, "parameter.relaxationCoefficientOfBoundaryCondition"	);

  FILE_skipComment(fp);//inflowboundaryCondition

  FILE_skipComment(fp);
  FILE_scanOnOff(fp, &parameter.flagOfInflowBoundaryCondition, "parameter.flagOfInflowBoundaryCondition");

  FILE_skipComment(fp);
  FILE_scanInt(fp, &parameter.inflowType, "parameter.inflowType");

  FILE_skipComment(fp);
  FILE_scanOnOff(fp, &parameter.flagOfAutoSettingOfInflowLevel, "parameter.flagOfAutoSettingOfInflowLevel");

	
  for (iDim = 0; iDim < 3; iDim++) {
	  FILE_skipComment(fp);
	  sprintf(variableName, "parameter.inflowPosition[%d]", iDim);
	  FILE_scanDouble(fp, &parameter.inflowPosition[iDim], variableName);
  }

  for (iDim = 0; iDim < 3; iDim++) {
	  FILE_skipComment(fp);
	  sprintf(variableName, "parameter.inflowVelocity[%d]", iDim);
	  FILE_scanDouble(fp, &parameter.inflowVelocity[iDim], variableName);
  }

  FILE_skipComment(fp);
  FILE_scanInt(fp, &parameter.numberOfExtraGhostParticles, "parameter.numberOfExtraGhostParticles");

  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.inflowMoleOfBubbles
	  , "parameter.inflowMoleOfBubbles");
  
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.inflowNumberOfBubbles
	  , "parameter.inflowNumberOfBubbles");



  FILE_skipComment(fp);
  for(iDim=0; iDim < 3; iDim++){
    FILE_skipComment(fp);
    sprintf(variableName,"physicalProperty.gravity[%d]",iDim);
    FILE_scanDouble( fp,&physicalProperty.gravity[iDim],variableName);
  }


  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.courantNumber
                     ,"parameter.courantNumber");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.diffusionNumber
                     ,"parameter.diffusionNumber");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&timer.maxDt_ratio
                     ,"timer.maxDt_ratio");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&timer.minDt_ratio
                     ,"timer.minDt_ratio");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&timer.upperLimitOfChangeRateOfDt
                     ,"timer.upperLimitOfChangeRateOfDt");

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.maxIterationNumberInIterationSolver
                  ,"parameter.maxIterationNumberInIterationSolver");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&parameter.minIterationNumberInIterationSolver
                  ,"parameter.minIterationNumberInIterationSolver");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.smallNumberForCheckingConvergenceInIterationSolver
                     ,"parameter.smallNumberForCheckingConvergenceInIterationSolver");

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.collisionDistance_ratio
                     ,"parameter.collisionDistance_ratio");

  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.collisionCoefficient
                     ,"parameter.collisionCoefficient");

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.thresholdOfParticleNumberDensity_ratio
                     ,"parameter.thresholdOfParticleNumberDensity_ratio");

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_scanDouble( fp,&parameter.relaxationCoefficientOfLambda
                     ,"parameter.relaxationCoefficientOfLambda");

  FILE_skipComment(fp);
  FILE_scanInt( fp,&timer.finishTimeStep
                     ,"timer.finishTimeStep");

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanOnOff( fp,&parameter.flagOfForcedMotionOfRigidBody
                    ,"parameter.flagOfForcedMotionOfRigidBody");

  FILE_skipComment(fp);
  FILE_scanInt(   fp,&parameter.typeNumberOfRigidParticle_forForcedMotion
                    ,"parameter.typeNumberOfRigidParticle_forForcedMotion");

  FILE_skipComment(fp);
  FILE_scanChar( fp, parameter.nameOfSamplingDataFileForForcedMotionOfRigidBody
                   ,"parameter.nameOfSamplingDataFileForForcedMotionOfRigidBody");
  
  FILE_skipComment(fp);
  FILE_skipComment(fp);
    
  FILE_scanOnOff( fp,&parameter.flagOfTanakaAndMasunagaModel
                   ,"parameter.flagOfTanakaAndMasunagaModel");
    
  FILE_skipComment(fp);
  FILE_scanDouble(   fp,&parameter.valueOfGamma
                 ,"parameter.valueOfGamma");
    
  FILE_skipComment(fp);
  FILE_scanDouble( fp, &parameter.valueOfC
                  ,"parameter.valueOfC");

  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanOnOff(fp, &parameter.flagOfKondoAndKoshizukaModel
	  , "parameter.flagOfKondoAndKoshizukaModel");
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.valueOfKondoAlpha
	  , "parameter.valueOfKondoAlpha");
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.valueOfKondoBeta
	  , "parameter.valueOfKondoBeta");
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.valueOfKondoGamma
	  , "parameter.valueOfKondoGamma");
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.artificialPressure
	  , "parameter.artificialPressure");
  FILE_skipComment(fp);
  FILE_scanDouble(fp, &parameter.radiusOfKondoCollision
	  , "parameter.radiusOfKondoCollision");


  FILE_skipComment(fp);//gradientTensor
  //FILE_scanChar( fp, parameter.nameOfSamplingDataFileForForcedMotionOfWall
  //                 ,"parameter.nameOfSamplingDataFileForForcedMotionOfWall");


  FILE_skipComment(fp);
  FILE_scanInt(fp, &parameter.flagOfGradientTensor
	  , "parameter.flagOfGradientTensor");
  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_scanOnOff(fp, &parameter.flagOfAveragePressureInEachBucket
	  , "parameter.flagOfAveragePressureInEachBucket");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &parameter.timeToUpdateAveragePressure
	  , "parameter.timeToUpdateAveragePressure");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureBucketWidth
	  , "domain.pressureBucketWidth");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureUpperLimit[XDIM]
	  , "&domain.pressureLowerLimit[XDIM]");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureUpperLimit[YDIM]
	  , "&domain.pressureUpperLimit[YDIM]");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureUpperLimit[ZDIM]
	  , "&domain.pressureUpperLimit[ZDIM]");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureLowerLimit[XDIM]
	  , "&domain.pressureLowerLimit[XDIM]");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureLowerLimit[YDIM]
	  , "&domain.pressureLowerLimit[YDIM]");
  FILE_skipComment(fp);

  FILE_scanDouble(fp, &domain.pressureLowerLimit[ZDIM]
	  , "&domain.pressureLowerLimit[ZDIM]");
  FILE_skipComment(fp);


  FILE_skipComment(fp);


  FILE_skipComment(fp);
  FILE_skipComment(fp);


  FILE_skipComment(fp);
  FILE_skipComment(fp);


  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_skipComment(fp);

  FILE_skipComment(fp);
  FILE_skipComment(fp);
  FILE_skipComment(fp);



  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_skipComment(fp);
  FILE_skipComment(fp);


  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_skipComment(fp);
  FILE_skipComment(fp);


  FILE_skipComment(fp);
  FILE_skipComment(fp);

  FILE_skipComment(fp);
  FILE_skipComment(fp);


  parameter.increaseRateOfDiagonalTermInCoefficientMatrixOfPressure = 2.0;

  parameter.flagOfExponentialExpressionInProfFile = OFF;


  parameter.flagOfDivisionOfVtkFile = ON;


  timer.intervalTimeOfDisplayingStateOfTimeStep   = 0.5;


  FILE_closeFile( fp, parameter.nameOfDataFile );

}


void
FILE_displayReadDataFile( void ){

  int  iType;
  int  iDim;

  fprintf(FpForLog,"\n");
  fprintf(FpForLog,"\n");
  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog,"        %s                                 \n",parameter.nameOfDataFile);
  fprintf(FpForLog,"====================================================\n");
  fprintf(FpForLog,"\n");

  fprintf(FpForLog,"#######--FREQUENTLY-USED-DATA-#####################################\n");
  fprintf(FpForLog,"#--------ParticleSize----------------------------------------------\n");
  fprintf(FpForLog,"averageDistanceBetweenParticles(m)                 %lf\n",particle.averageDistance);
  fprintf(FpForLog,"#--------Time------------------------------------------------------\n");
  fprintf(FpForLog,"finishTime(sec)                                    %lf\n",timer.finishTime);
  fprintf(FpForLog,"inititalDt(sec)                                    %lf\n",timer.dt_initial);
  fprintf(FpForLog,"#--------File------------------------------------------------------\n");
  fprintf(FpForLog,"TypeOfOutputInterval(simulationTime/timeStep)      %s\n",FILE_returnTypeOfOutputInterval(timer.typeOfOutputInterval));
  fprintf(FpForLog,"--simulationTime---interval(sec)                   %lf\n",timer.intervalTimeOfWritingProfFile_simulationTime);
  fprintf(FpForLog,"--timeStep---------interval(timeStep)              %d\n",timer.intervalTimeOfWritingProfFile_timeStep);
  fprintf(FpForLog,"#--------Domain----------------------------------------------------\n");
  fprintf(FpForLog,"numberOfDimensions(2or3)                           %d\n",NumberOfDimensions);
  fprintf(FpForLog,"autoSettingOfDomainSize(on/off)                    %s\n",FILE_returnOnOff(domain.flagOfAutoSettingOfDomainSize));

  for(iDim=0; iDim < 3; iDim++){
    fprintf(FpForLog,"--on---upperMarginRatio[%s](ratio)               %lf\n",FILE_returnDim(iDim),domain.upperMarginRatio[iDim]);
  }

  for(iDim=0; iDim < 3; iDim++){
    fprintf(FpForLog,"--on---lowerMarginRatio[%s](ratio)               %lf\n",FILE_returnDim(iDim),domain.lowerMarginRatio[iDim]);
  }

  for(iDim=0; iDim < 3; iDim++){
    fprintf(FpForLog,"--off--upperLimit[%s](m)                         %lf\n",FILE_returnDim(iDim),domain.upperLimit[iDim]);
  }

  for(iDim=0; iDim < 3; iDim++){
    fprintf(FpForLog,"--off--lowerLimit[%s](m)                         %lf\n",FILE_returnDim(iDim),domain.lowerLimit[iDim]);
  }

  fprintf(FpForLog,"#--------RadiusOfInteractionDomain--------------------------------\n");
  fprintf(FpForLog,"radiusOfParticleNumberDensity(ratio)               %lf\n",parameter.radiusOfParticleNumberDensity_ratio);
  fprintf(FpForLog,"radiusOfGradient(ratio)                            %lf\n",parameter.radiusOfGradient_ratio);
  fprintf(FpForLog,"radiusOfLaplacianForViscosity(ratio)               %lf\n",parameter.radiusOfLaplacianForViscosity_ratio);
  fprintf(FpForLog,"radiusOfLaplacianForPressure(ratio)                %lf\n",parameter.radiusOfLaplacianForPressure_ratio);
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#######--LESS-FREQUENTLY-USED-DATA-###############################\n");
  fprintf(FpForLog,"#--------Bucket(subDomain)----------------------------------------\n");
  fprintf(FpForLog,"autoSettingOfBucketCapacity                        %s\n",FILE_returnOnOff(domain.flagOfAutoSettingOfBucketCapacity));
  fprintf(FpForLog,"--on---optimizationOfMemorySize(on/off)            %s\n",FILE_returnOnOff(domain.flagOfOptimizationOfBucketMemorySize));
  fprintf(FpForLog,"--on-----off--marginRatio(ratio)                   %lf\n",domain.marginRatioForSettingBucketCapacity);
  fprintf(FpForLog,"--off--capacityOfBucket                            %d\n",domain.capacityOfBucket);
  fprintf(FpForLog,"#--------TableOfNeighborParticles---------------------------------\n");
  fprintf(FpForLog,"theNumberOfNeighborTables(1or2)                    %d\n",parameter.numberOfNeighborTables);
  fprintf(FpForLog,"autoSettingOfCapacityOfNeighborTable               %s\n",FILE_returnOnOff(parameter.flagOfAutoSettingOfCapacityOfNeighborTable));
  fprintf(FpForLog,"--on---marginRatioOfLargeTable(ratio)              %lf\n",parameter.marginRatioForSettingCapacityOfLargeNeighborTable);
  fprintf(FpForLog,"--on---marginRatioOfSmallTable(ratio)              %lf\n",parameter.marginRatioForSettingCapacityOfSmallNeighborTable);
  fprintf(FpForLog,"--off--capacityOfLargeNeighborTable                %d\n",parameter.capacityOfNeighborTable_large);
  fprintf(FpForLog,"--off--capacityOfSmallNeighborTable                %d\n",parameter.capacityOfNeighborTable_small);

  fprintf(FpForLog,"#--------ProfFile-------------------------------------------------\n");
  fprintf(FpForLog,"divisionOfProfFile(on/off)                         %s\n",FILE_returnOnOff(parameter.flagOfDivisionOfProfFile));
  fprintf(FpForLog,"--on---compressionOfProfFile                       %s\n",FILE_returnOnOff(parameter.flagOfCompressionOfProfFile));
  fprintf(FpForLog,"--on---upperLimitOfNumberOfFiles                   %d\n",parameter.upperLimitOfNumberOfProfFiles);

  fprintf(FpForLog,"#--------PressureFile---------------------------------------------\n");
  fprintf(FpForLog,"outputPressureFile(on/off)                         %s\n",FILE_returnOnOff(parameter.flagOfOutputOfPressureFile));
  fprintf(FpForLog,"--on---outputPressureOfAllWallParticle(on/off)     %s\n",FILE_returnOnOff(parameter.flagOfOutputOfAllWallParticle));
  fprintf(FpForLog,"---------off--nameOfInputFile                      %s\n",parameter.nameOfDesignationFileForWritingPressureFile);
  fprintf(FpForLog,"--on---divisionOfPressureFile(on/off)              %s\n",FILE_returnOnOff(parameter.flagOfDivisionOfPressureFile));
  fprintf(FpForLog,"---------on---upperLimitOfNumberOfFiles            %d\n",parameter.upperLimitOfNumberOfPressureFiles);
  fprintf(FpForLog,"---------on---nameOfOutputFile                     %s\n",parameter.nameOfOutputPressureFile_divided);
  fprintf(FpForLog,"---------on---nameOfOutputFile                     %s\n",parameter.nameOfOutputPressureFile);
    
  fprintf(FpForLog,"#--------TorqueFile---------------------------------------------\n");
  fprintf(FpForLog,"outputTorqueFile(on/off)                         %s\n",FILE_returnOnOff(parameter.flagOfOutputOfTorqueFile));
  //fprintf(FpForLog,"--on---outputTorqueOfAllRigidParticle(on/off)     %s\n",FILE_returnOnOff(parameter.flagOfOutputOfAllRigidParticle));
  //fprintf(FpForLog,"---------off--nameOfInputFile                      %s\n",parameter.nameOfDesignationFileForWritingTorqueFile);
  //fprintf(FpForLog,"--on---divisionOfTorqueFile(on/off)              %s\n",FILE_returnOnOff(parameter.flagOfDivisionOfTorqueFile));
  //fprintf(FpForLog,"---------on---upperLimitOfNumberOfFiles            %d\n",parameter.upperLimitOfNumberOfTorqueFiles);
  //fprintf(FpForLog,"---------on---nameOfOutputFile                     %s\n",parameter.nameOfOutputTorqueFile_divided);
  fprintf(FpForLog,"---------on---nameOfOutputFile                     %s\n",parameter.nameOfOutputTorqueFile);

  fprintf(FpForLog,"#--------TypeOfParticle-------------------------------------------\n");
  fprintf(FpForLog,"numberOfParticleTypes                              %d\n",parameter.numberOfParticleTypes);
  fprintf(FpForLog,"--typeNumberOfWallParticle                         %d\n",parameter.wallType);
  fprintf(FpForLog,"--typeNumberOfDummyWallParticle                    %d\n",parameter.dummyWallType);

  fprintf(FpForLog,"#--------MassDensity----------------------------------------------\n");
  for(iType=0; iType < parameter.numberOfParticleTypes; iType++){
    fprintf(FpForLog,"massDensityOfType%d(kg/m^3)                         %lf\n",iType, physicalProperty.massDensity[iType]);
  }

  fprintf(FpForLog,"#--------Compressibility------------------------------------------\n");
  for(iType=0; iType < parameter.numberOfParticleTypes; iType++){
    fprintf(FpForLog,"compressibilityOfType%d                             %e\n",iType, physicalProperty.compressibility[iType]);
  }

  fprintf(FpForLog,"#--------Viscosity------------------------------------------------\n");
  fprintf(FpForLog,"flagOfViscosityCalculation(on/off)                 %s\n", FILE_returnOnOff(parameter.flagOfViscosityCalculation));

  fprintf(FpForLog,"--on--flagOfHighViscosityCalculation(on/off)       %s\n", FILE_returnOnOff(parameter.flagOfHighViscosityCalculation));

  fprintf(FpForLog,"--on--kinematicViscosity(m^2/s)                    %e\n", physicalProperty.kinematicViscosity);
    
  fprintf(FpForLog,"#--------Bubble------------------------------------------------\n");
  fprintf(FpForLog,"flagOfBubbleCalculationy(on/off)                  %s\n", FILE_returnOnOff(parameter.flagOfBubbleCalculation));
  fprintf(FpForLog,"--on---nameOfInputfile                            %s\n", parameter.nameOfBubbleInputFile);
  fprintf(FpForLog,"--on--massDensityOfBubble(kg/m^3)                 %e\n", physicalProperty.massDensityOfBubble);
  fprintf(FpForLog,"--on--gasConstant(J/K*mol)		                  %e\n", physicalProperty.gasConstant);
  fprintf(FpForLog,"--on--temperature(K)                              %e\n", physicalProperty.temperature);
  fprintf(FpForLog,"--on--headPressure(Pa)                            %e\n", physicalProperty.headPressure);

  fprintf(FpForLog,"#--------OutflowBoundaryCondition----------------------------------\n");
  fprintf(FpForLog,"flagOfOutflowBoundaryCondition(on/off)              %s\n", FILE_returnOnOff(parameter.flagOfOutflowBoundaryCondition));
  fprintf(FpForLog,"negativePressure(on/off)                           %s\n", FILE_returnOnOff(parameter.flagOfNegativePressure));
  for (iDim = 0; iDim < 3; iDim++) {
	  fprintf(FpForLog, "--on--positionVector[%s]                         %lf\n", FILE_returnDim(iDim), parameter.positionVector[iDim]);
  }
  for (iDim = 0; iDim < 3; iDim++) {
	  fprintf(FpForLog, "--on--normalVector[%s]                         %lf\n", FILE_returnDim(iDim), parameter.normalVector[iDim]);
  }
  fprintf(FpForLog, "--on--LengthOfFixedVelocityRegion(ratio)           %lf\n", parameter.lengthOfFixedVelocityRegion);
  fprintf(FpForLog, "--on--LengthOfOutflow(m)                            %lf\n", parameter.lengthOfOutflow);
  fprintf(FpForLog,"--on--widthOfOutflow(m)                             %lf\n", parameter.widthOfOutflow);
  fprintf(FpForLog, "--on--depthOfOutflow(m)                             %lf\n", parameter.depthOfOutflow);
  fprintf(FpForLog,"--on--relaxationCoefficient                        %lf\n", parameter.relaxationCoefficientOfBoundaryCondition);

  fprintf(FpForLog, "#--------InflowBoundaryCondition-----------------------------------\n");
  fprintf(FpForLog, "flagOfInflowBoundaryCondition(on/off)               %s\n", FILE_returnOnOff(parameter.flagOfInflowBoundaryCondition));
  fprintf(FpForLog, "--on--typeNumberOfInflowParticle                   %d\n", parameter.inflowType);
  fprintf(FpForLog, "--on--autoSettingOfInflowLevel(on/off)             %s\n", FILE_returnOnOff(parameter.flagOfAutoSettingOfInflowLevel));
  for (iDim = 0; iDim < 3; iDim++) {
	  fprintf(FpForLog, "--------on--positionVector[%s]                   %lf\n", FILE_returnDim(iDim), parameter.inflowPosition[iDim]);
  }
  for (iDim = 0; iDim < 3; iDim++) {
	  fprintf(FpForLog, "--on--inflowVelocity[%s](m/s)                  %lf\n", FILE_returnDim(iDim), parameter.inflowVelocity[iDim]);
  }
  fprintf(FpForLog, "--on--numberofExtraGhostParticles                  %d\n", parameter.numberOfExtraGhostParticles);
  fprintf(FpForLog, "--on--inflowMoleOfBubbles                          %e\n", parameter.inflowMoleOfBubbles);
  fprintf(FpForLog, "--on--inflowNumberOfBubbles                        %lf\n", parameter.inflowNumberOfBubbles);


  fprintf(FpForLog,"#--------Gravity--------------------------------------------------\n");
  for(iDim=0; iDim < 3; iDim++){
    fprintf(FpForLog,"gravity[%s](m/s^2)                               %lf\n",FILE_returnDim(iDim),physicalProperty.gravity[iDim]);
  }

  fprintf(FpForLog,"#--------TimeDifference-------------------------------------------\n");
  fprintf(FpForLog,"courantNumber                                      %lf\n",parameter.courantNumber);
  fprintf(FpForLog,"diffusionNumber                                    %lf\n",parameter.diffusionNumber);
  fprintf(FpForLog,"maxDt(ratio)                                       %lf\n",timer.maxDt_ratio);
  fprintf(FpForLog,"minDt(ratio)                                       %lf\n",timer.minDt_ratio);
  fprintf(FpForLog,"upperLimitOfChangeRateOfDt(ratio)                  %lf\n",timer.upperLimitOfChangeRateOfDt);

  fprintf(FpForLog,"#--------SolvelerOfSimultaneousEquations--------------------------\n");
  fprintf(FpForLog,"upperLimitOfIterationNumber                        %d\n",parameter.maxIterationNumberInIterationSolver);
  fprintf(FpForLog,"lowerLimitOfIterationNumber                        %d\n",parameter.minIterationNumberInIterationSolver);
  fprintf(FpForLog,"smallNumberForCheckingConvergence                  %e\n",parameter.smallNumberForCheckingConvergenceInIterationSolver);

  fprintf(FpForLog,"#--------CollisionBetweenParticles--------------------------------\n");
  fprintf(FpForLog,"collisionDistance(ratio)                           %lf\n",parameter.collisionDistance_ratio);
  fprintf(FpForLog,"collisionCoefficient                               %lf\n",parameter.collisionCoefficient);

  fprintf(FpForLog,"#--------DirichletBoundaryCondition-------------------------------\n");
  fprintf(FpForLog,"thresholdOfParticleNumberDensity(ratio)            %lf\n",parameter.thresholdOfParticleNumberDensity_ratio);


  fprintf(FpForLog,"#--------Other----------------------------------------------------\n");
  fprintf(FpForLog,"relaxationCoefficientOfLambda                        %lf\n",parameter.relaxationCoefficientOfLambda);
  fprintf(FpForLog,"finishTimeStep(timeStep)                           %d\n",timer.finishTimeStep);


  fprintf(FpForLog,"######---CustomizedFunctions---###################################\n");
  fprintf(FpForLog,"#--------RigidBody------------------------------------------------\n");
  fprintf(FpForLog,"forcedMotionOfRigidBody(on/off)                    %s\n",FILE_returnOnOff(parameter.flagOfForcedMotionOfRigidBody));
  fprintf(FpForLog,"--on---typeNumberOfRigidParticle                   %d\n",parameter.typeNumberOfRigidParticle_forForcedMotion);
  fprintf(FpForLog,"--on---fileNameOfSamplingDataForRigidBody          %s\n",parameter.nameOfSamplingDataFileForForcedMotionOfRigidBody);
  fprintf(FpForLog,"#--------TANAKAandMASUNAGA------------------------------------------------\n");
    fprintf(FpForLog,"flagOfTanakaAndMasunagaModel(on/off)                    %s\n",FILE_returnOnOff(parameter.flagOfTanakaAndMasunagaModel));
    fprintf(FpForLog,"--on--valueOfGamma                   %lf\n",parameter.valueOfGamma);
    fprintf(FpForLog,"--on--valueOfC          %lf\n",parameter.valueOfC);
  fprintf(FpForLog, "#--------KONDOandKOSHIZUKAModel---------------------------------- - \n");
  fprintf(FpForLog,"flagOfKondoAndKoshizukaModel(on/off)               %s\n",FILE_returnOnOff(parameter.flagOfKondoAndKoshizukaModel));
  fprintf(FpForLog, "--on---valueOfKondoAlpha                                %lf\n", parameter.valueOfKondoAlpha);
  fprintf(FpForLog, "--on---valueOfKondoBeta                                 %lf\n", parameter.valueOfKondoBeta);
  fprintf(FpForLog, "--on---valueOfKondoGamma                                %lf\n", parameter.valueOfKondoGamma);
  fprintf(FpForLog, "--on---artificialPressure                               %lf\n", parameter.artificialPressure);
  fprintf(FpForLog, "--on---radiusOfCollision(ratio)                         %lf\n", parameter.radiusOfKondoCollision);
  fprintf(FpForLog,"#--------GradientTensor-------------------------------------------\n");
  fprintf(FpForLog,"flagOfGradientTensor(on/off)                       %s\n", FILE_returnOnOff(parameter.flagOfGradientTensor));
  fprintf(FpForLog,"#--------AveragePressure------------------------------------------\n");
  fprintf(FpForLog,"flagOfAveragePressureInEachBucket(on/off)          %s\n", FILE_returnOnOff(parameter.flagOfAveragePressureInEachBucket));
  fprintf(FpForLog,"--on---timeToStartUpdateAveragePressure            %lf\n", parameter.timeToUpdateAveragePressure);
  fprintf(FpForLog,"--on---bucketWidth(m)                              %lf\n", domain.pressureBucketWidth);
  fprintf(FpForLog,"--on---upperLimit[XDIM](m)                         %lf\n", domain.pressureUpperLimit[XDIM]);
  fprintf(FpForLog,"--on---upperLimit[YDIM](m)                         %lf\n", domain.pressureUpperLimit[YDIM]);
  fprintf(FpForLog,"--on---upperLimit[ZDIM](m)                         %lf\n", domain.pressureUpperLimit[ZDIM]);
  fprintf(FpForLog,"--on---lowerLimit[XDIM](m)                         %lf\n", domain.pressureLowerLimit[XDIM]);
  fprintf(FpForLog, "--on---lowerLimit[YDIM](m)                         %lf\n", domain.pressureLowerLimit[YDIM]);
  fprintf(FpForLog, "--on---lowerLimit[ZDIM](m)                         %lf\n", domain.pressureLowerLimit[ZDIM]);

  fprintf(FpForLog,"#-----------------------------------------------------------------\n");

  fprintf(FpForLog,"#-----------------------------------------------------------------\n");

  fprintf(FpForLog,"#-----------------------------------------------------------------\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"#############                                      ***\n");
  fprintf(FpForLog,"\n\n\n");

}

