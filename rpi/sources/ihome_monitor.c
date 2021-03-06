/**
 *  send monitoring message to IoT server.
 *
 *  @Module   ihome_monitor
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
 *   - January 13, 2015
 *   - first issue
 */

// avoid implicit declaration of nanosleep
#define _POSIX_C_SOURCE 199309L

#include <unistd.h>
#include <sys/time.h>
#include "ihome_public.h"

struct hostent *server;
struct sockaddr_in serv_addr;
int bytes, sent, received, total, l_indx;
boolean_t l_socket_failed;

/* sent and received messages */
char message[1024], response[500], *e1, *e2;

/* http post request fields */
//char *private_key = "2mP7ZjdbvVcbn8m92Vm9" ;
char input_buffer  [ 2 * nb_Of_Input_Elements - 1 ];
char output_buffer [ 2 * nb_Of_Output_Elements - 1 ];
char message_buffer [ 57 * nb_OF_ACTIVE_MESSAGES - 2 ]; // 55 char per message + 2 for , & space

void *ihome_monitor ( void *prm)
{

  int l_indx ;
  nanosleep((struct timespec[]) {{2, 0}}, NULL);

  /* connect the socket */
  /*  if ( connect(socket_monitor, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {print_error(0,"connection")}
  */
  while (1) {
    /* fill in the parameters : input_buffer */
    memset(input_buffer, ':', sizeof(input_buffer));
    for (l_indx = 0 ; l_indx < nb_Of_Input_Elements ; l_indx++)
    {
      *(input_buffer + l_indx * 2) = inputs_Array_Of_Elements[l_indx].value + 48;
    }
    //log_print("set the input buffer to \':\' \n");

    /* fill in the parameters : output_buffer */
    memset(output_buffer, ':', sizeof(output_buffer));
    for (l_indx = 0 ; l_indx < nb_Of_Output_Elements ; l_indx++)
    {
      *(output_buffer + l_indx * 2) = outputs_Array_Of_Elements[l_indx].value + 48 ;
    }
    //log_print("set the output buffer to \':\' \n");

    /* create the socket */
    socket_monitor = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname(host);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    //log_print("create the monitoring socket\n");

    //l_socket_failed = FALSE ;
    /* connect the socket */
    if (connect(socket_monitor, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      /* close the socket */
      close(socket_monitor);
      //l_socket_failed = TRUE ;
      log_print("monitor socket failed to connect\n");
      continue ;
    }

    //if(l_socket_failed == FALSE )
    //{
    /* fill in the parameters : message_buffer */
    memset(message_buffer, 0, sizeof(message_buffer));
    boolean_t no_message_to_send = TRUE ;
    for (l_indx = 0 ; l_indx < nb_OF_ACTIVE_MESSAGES - 1 ; l_indx++)
    {
      if ( active_message_list[l_indx].id_message != NO_ACTIVE_MESSAGE )
      {
        strcat(message_buffer, messages_list_cst[active_message_list[l_indx].id_message].monitor_message.message );
        strcat(message_buffer, ",+");
        active_message_list[l_indx].sent_to_server = TRUE ;
        no_message_to_send = FALSE ;
      }
    }

    if ( active_message_list[l_indx].id_message != NO_ACTIVE_MESSAGE )
    {
      strcat(message_buffer, messages_list_cst[active_message_list[l_indx].id_message].monitor_message.message  );
    }
    if (no_message_to_send == TRUE)
    {
      strcat(message_buffer, messages_list_cst[NO_ACTIVE_MESSAGE].monitor_message.message  );
    }
    //log_print("fill the message buffer with monitoring messages\n");

    memset(message, 0, sizeof(message));
    sprintf(message, http_post_request, private_key, input_buffer, message_buffer, output_buffer);
    //log_print("fill the post message to send to sparkfun data server\n");

    /* send the request */
    total = strlen(message);
//  sent = 0;
//  do {
//      bytes = write(socket_monitor,message+sent,total-sent);
    send(socket_monitor, message, total, 0);
    //   log_print("send message via socket_monitor\n");

    /*  if (bytes < 0)
      {
    print_error(0,"write");
      }
      if (bytes == 0)
          break;
      sent+=bytes;
    } while (sent < total);
    */
    /* receive the response */
    memset(response, 0, sizeof(response));
    total = sizeof(response) - 1;
    received = 0;
//  do {
//      bytes = read(socket_monitor,response+received,total-received);
    recv(socket_monitor, response, total, 0);
    //       log_print("receive response via socket_monitor\n");

    /*      if (bytes < 0)
          {
         close(socket_monitor);
    //  print_error(0,"read")
          }
          if (bytes == 0)
              break;
          received+=bytes;
      } while (received < total);
    */
    if (received >= total)
    {
//    print_error(0,"overflow")
      log_print("received data > total in monitor task \n");
      close(socket_monitor);
      continue ;
    }

    //log_print("process response of monitor socket\n");
    /* process response */
    e1 = strstr(response, "\r\n\r\n");
    if (e1 != NULL)
    {
      e2 = strstr(e1, "1");
      if (e2 == NULL)
        e2 = strstr(e1, "0");

      if (e2[0] == '0')
      {
        for (l_indx = 0 ; l_indx < nb_OF_ACTIVE_MESSAGES ; l_indx++)
        {
          if ( active_message_list[l_indx].id_message == NO_ACTIVE_MESSAGE )
          {
            active_message_list[l_indx].id_message = MESSAGE_6 ;
            l_indx = nb_OF_ACTIVE_MESSAGES ;
          }
          else if ( active_message_list[l_indx].id_message == MESSAGE_6 )
          {
            l_indx = nb_OF_ACTIVE_MESSAGES ;
          }
        }
      }
      else if (e2[0] == '1')
      {
        for (l_indx = 0 ; l_indx < nb_OF_ACTIVE_MESSAGES ; l_indx++)
        {
          if ( active_message_list[l_indx].id_message != NO_ACTIVE_MESSAGE )
          {
            pthread_mutex_lock(&active_message_list[l_indx].mutex);
            active_message_list[l_indx].sent_to_server = TRUE ;
            pthread_mutex_unlock(&active_message_list[l_indx].mutex);
          }
        }
      }

    }
    nanosleep((struct timespec[]) {{30, 0}}, NULL);

  }
}
