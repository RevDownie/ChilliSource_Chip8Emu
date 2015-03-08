//
//  OpCodeActions.h
//  CSChip8Emulator
//
//  Created by Scott Downie on 28/02/2015.
//  Copyright (c) 2015 Tag Games Ltd. All rights reserved.
//

#ifndef _OPCODEACTIONS_H_
#define _OPCODEACTIONS_H_

#include <ChilliSource/ChilliSource.h>

#include "Chip8MutableState.h"

//NOTE: Opcode actions found here: http://en.wikipedia.org/wiki/CHIP-8#Opcode_table

using OpCode = u16;

namespace OpCodeActions
{
    //Use in place of unknown OpCode
    void NoOp(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Clear the screen
    void x00E0(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Jump program to NNN
    void x1NNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Jump to NNN and push the stack (call subroutine)
    void x2NNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Return from subroutine
    void x00EE(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Skip instruction if VX == NN
    void x3XNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Skip instruction if VX != NN
    void x4XNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Skip instruction if VX != VY
    void x5XY0(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to NN
    void x6XNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Add NN to VX
    void x7XNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to VY
    void x8XY0(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to VX | VY
    void x8XY1(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to VX & VY
    void x8XY2(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to VX ^ VY
    void x8XY3(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Add VY to VX and store overflow in carrt
    void x8XY4(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Subtract VY from VX
    void x8XY5(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Shift VX right
    void x8XY6(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to VY - VX
    void x8XY7(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Shift VX left
    void x8XYE(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Skip instruction if VX != VY
    void x9XY0(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set I to NNN
    void xANNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Jump program to NNN + V0
    void xBNNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to random masked by NN
    void xCXNN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Toggle pixel at X, Y on or off
    void xDXYN(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Skip instruction if key at VX is pressed
    void xEX9E(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Skip instruction if key at VX is not pressed
    void xEXA1(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set VX to delay timer
    void xFX07(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set delay timer to VX
    void xFX15(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set sound timer to VX
    void xFX18(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Store binary coded VX to memory
    void xFX33(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Add VX to I
    void xFX1E(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Await key press
    void xFX0A(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Set I to location of sprite at VX
    void xFX29(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Store all V in memory
    void xFX55(OpCode in_opCode, Chip8MutableState& inout_state);
    
    //Fetch all V from memory
    void xFX65(OpCode in_opCode, Chip8MutableState& inout_state);
}

#endif
