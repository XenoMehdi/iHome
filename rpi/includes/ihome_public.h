/**
 *  API for iHome Software.
 *
 *  @Module   ihome_public
 *  @author   El Mehdi El Fakir
 *  @email    elmehdi@elfakir.me
 *  @website  --
 *  @link     --
 *  @version  v1.0
 *  @compiler GCC
 *  @hardware Raspberry Pi B+
 *  @license  GNU GPL v3
 *
 * |----------------------------------------------------------------------
 * | Copyright (C) FEZ EMBEDDED SYSTEMS INDUSTRY, 2015
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *
 *  Version 1.0
 *   - January 08, 2015
 *   - first issue
 *
 */
#ifndef IHOME_PUBLIC_H
#define IHOME_PUBLIC_H

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <bcm2835.h>

#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

/**********************/
/* Type Declaration   */
/**********************/
// Define the boolean type
typedef unsigned char boolean_t;
#define TRUE ( 1 == 1 )
#define FALSE ( 1 == 0 )

#define print_error(x,s)  printf("ERROR writing message to socket {%s}\n",s); exit(x);
#define log_print(x) {  time_t t = time(NULL); \
      struct tm tm = *localtime(&t); \
      log_file_desc = fopen("/root/log/log.txt", "a+"); \
      if(log_file_desc != NULL) {\
      fprintf(log_file_desc,"[%d/%d/%d %d:%d:%d] %s",tm.tm_mday, tm.tm_mon+1, tm.tm_year +1900, tm.tm_hour, tm.tm_min, tm.tm_sec, x); \
      fclose(log_file_desc);} \
      }

#define nb_OF_ACTIVE_MESSAGES 7
#define nb_Of_Config_Elements 1

#define ERR_CONNECT_SOCKET  0x01
#define ERR_OPEN_HOST       0x02
#define ERR_OPEN_SOCKET     0x04
#define LCD_INIT_FAIL       0x08

// define the structure of input object
typedef struct {
  boolean_t       value ;
  boolean_t       validity ;
  pthread_mutex_t mutex ;
} input_object_t ;

typedef enum {
  input_1,
  input_2,
  input_3,
  input_4,
  input_5,
  input_6,
  input_7,
  input_8,
  nb_Of_Input_Elements
} input_element_t ;

typedef enum {
  command_1,
  command_2,
  command_3,
  command_4,
  command_5,
  command_6,
  command_7,
  command_8,
  nb_Of_Command_Elements
} command_element_t ;

// define the structure of output object
typedef struct {
  boolean_t       value ;
  boolean_t       validity ;
  pthread_mutex_t mutex ;
} output_object_t ;

typedef enum {
  output_RLY_1,
  output_RLY_2,
  output_RLY_3,
  output_RLY_4,
  output_RLY_5,
  output_RLY_6,
  output_RLY_7,
  output_RLY_8,
  nb_Of_Output_Elements
} output_element_t ;

// define the structure of configuration object
typedef enum {
  NORMAL_RELAY,
  PERIODIC_TRIGGERING
} output_type_t ;

// define the structure of configuration object
typedef struct {
  output_type_t type_of_output  ;
  boolean_t     active_on_high  ;
  unsigned int  time_before_activation ;
  unsigned int  time_before_deactivation ;
  unsigned char repetition       ;
  unsigned int  interval_minutes ;
  unsigned int  time_of_set ;
  unsigned int  time_of_reset  ;
} config_object_t ;

// define the structure of contexte object
typedef struct context_object_s {
  input_object_t *input ;
  output_object_t *output ;
  config_object_t *config ;
  struct context_object_s *prev ;
  struct context_object_s *next ;
} context_object_t ;

// define an lcd message structure
typedef struct {
  char lcd_line_1 [17] ;
  char lcd_line_2 [17] ;
} lcd_message_t ;

// define a monitoring message structure
typedef struct {
  char message [255] ;
} monitor_message_t ;

typedef enum {
  NO_ACTIVE_MESSAGE,
  MESSAGE_1,
  MESSAGE_2,
  MESSAGE_3,
  MESSAGE_4,
  MESSAGE_5,
  MESSAGE_6,
  MESSAGE_7,
  MESSAGE_8,
  MESSAGE_9,
  MESSAGE_10,
  nb_Of_Messages
} messages_elements_t ;

/* define a structure of message that contains a field for lcd_message      */
/* and a field for monitoring_message with the same meaning but with        */
/* different density, monitoring_message is more detailled than lcd_message */
typedef struct {
  lcd_message_t         lcd_message ;
  monitor_message_t     monitor_message ;
} messages_object_t ;

typedef struct {
  messages_elements_t   id_message ;
  boolean_t             printed_to_lcd ;
  boolean_t             sent_to_server ;
  pthread_mutex_t       mutex ;
} active_message_t ;

typedef enum {
  OFF,
  ON
} pin_state_t ;

// define a structure for software configuration
typedef  union {
  unsigned int word ;
  struct {
    pin_state_t turn_LCD_Power      : 1 ;
    pin_state_t turn_LCD_Backlight  : 1 ;
  } options ;
} sw_configuration_t ;

// Define configuration structure
/**********************/
/* Public Data        */
/**********************/
// threads
extern pthread_t write_thread ;
extern pthread_t read_thread ;
extern pthread_t monitor_thread ;
extern pthread_t update_thread ;

// Arrays of I/O pins
extern unsigned int  lcd_pins [] ;
extern unsigned int  pins_in  [nb_Of_Input_Elements]  ;
extern unsigned int  pins_out [nb_Of_Output_Elements] ;

// lcd handler
extern int lcd_handler;

// variable to handle the sigterm signal, if = false the main while should stop and programm stops
extern boolean_t program_is_running ;

// Array of input elements.
extern input_object_t inputs_Array_Of_Elements [ nb_Of_Input_Elements ];
extern input_object_t commands_Array_Of_Elements [ nb_Of_Command_Elements ];
extern const input_object_t input_object_cst;

// Array of output elements.
extern output_object_t outputs_Array_Of_Elements [ nb_Of_Output_Elements ];
extern const output_object_t output_object_cst;

// List of defined messages
extern const messages_object_t messages_list_cst [ nb_Of_Messages ] ;

// list of active messages to send to server and to print on LCD
extern active_message_t active_message_list [ nb_OF_ACTIVE_MESSAGES] ;
extern const active_message_t active_message_init_cst ;

// active software configuration
extern sw_configuration_t software_configuration ;

// default software configuration
extern const sw_configuration_t software_configuration_default ;

// LCD driver handler
extern int  lcd_handler ;

// initial context object
extern config_object_t config_Array_Of_Elements[nb_Of_Config_Elements];
extern context_object_t *context_pile;
extern context_object_t context_1, context_2, context_3, context_4 ;

/**********************/
/* Public Functions   */
/**********************/

extern int   ihome_initialize ( void ) ;
extern void *ihome_monitor    ( void *prm) ;
extern void *ihome_read       ( void *prm) ;
extern void *ihome_update     ( void *prm) ;
extern void *ihome_write      ( void *prm) ;


/* Socket data */
extern int   port ;
extern char *host ;
extern char *http_post_request ;
extern char *http_get_request ;
extern char *private_key ;

extern int   socket_monitor ;
extern int   socket_read ;

/* Log file descriptor */
extern FILE *log_file_desc;

// functions to run when a signal is caught
extern void ihome_signal_term();
extern void ihome_signal_hup();
extern void ihome_signal_int();

#endif
