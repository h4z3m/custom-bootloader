# CURT Bootloader for STM32F103

## Communication procedures

Specific communication protocol is abstracted from the bootloader. Any appropriate protocol can be used to communicate with the bootloader. The only thing that is required is overloading the weak functions 'BL_send' and 'BL_receive' that are used internally for the command handling. Other vendor specific functions are declared as weak functions to allow this bootloader to be more flexible.

Currently, CURT_BL supports supports these commands:
    - BL_MEM_WRITE_CMD
      - Writes the received data to flash memory
    - BL_VER_CMD
      - Sends version information
    - BL_FLASH_ERASE_CMD
      - Erases flash memory
    - BL_ENTER_CMD_MODE_CMD
      - Prompts the bootloader to enter command mode
    - BL_JUMP_TO_APP_CMD
      - Jumps to application
    - BL_DATA_PACKET_CMD
      - Data packet command, used to send data during flashing or reading
    - BL_ACK_CMD
      - Acknowledge command
    - BL_RESPONSE_CMD
      - Response command

### BL_VER_CMD Procedure

1. Host sends BL_VER_CMD
2. BL sends BL_ACK_CMD
   1. If failed, BL sends BL_ACK_CMD with negative ack with the errored field.
3. BL ends BL_RESPONSE_CMD with the version info at data[0]

### BL_MEM_WRITE_CMD Procedure

1. Host sends BL_MEM_WRITE_CMD.
2. BL sends BL_ACK_CMD.
   1. If failed, BL sends BL_ACK_CMD with negative ack with the errored field.
3. When the host receives positive ACK, it must send data blocks to BL:
   1. For every block successfully received, the BL writes it to memory, then sends a positive ACK.
   2. If the block is corrupted, a negative ack is sent, with the errored field set and the procedure is aborted.
   3. For the last block, the host must set the 'end_flag' field to '1' to indicate the end of the memory read.

### BL_FLASH_ERASE_CMD Procedure

1. Host sends BL_FLASH_ERASE_CMD with the start address and pages to erase starting from thet address.
2. BL sends BL_ACK_CMD.
   1. If failed, BL sends BL_ACK_CMD with negative ack with the errored field.


### BL_ENTER_CMD_MODE_CMD Procedure

1. Host sends synchronization byte then BL_ENTER_CMD_MODE_CMD with a special key value.
2. BL sends BL_ACK_CMD.
   1. If failed, BL sends BL_ACK_CMD with negative ack with the errored field.

## TODO

1. Add more commands
2. Implement a better error handling
3. Test functions
4. Use protobuffers for communication
5. Bootloader code protection
6. Optimization for size
7. Debugging logs
