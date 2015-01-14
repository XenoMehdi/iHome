/**
 *  send monitoring message to IoT server.
 *
 *  @Module   ihome_monitor
 *	@author 	El Mehdi El Fakir
 *	@email		elmehdi@elfakir.me
 *	@website	--
 *	@link		  --
 *	@version 	v1.0
 *	@compiler GCC
 *  @hardware Raspberry Pi B+
 *	@license	GNU GPL v3
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
 *	Version 1.0
 *	 - January 13, 2015
 *	 - first issue
 */

#include "ihome_public.h"
#include "ihome_private_private.h"

#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* read, write, close */

void *ihome_monitor ( void *prm)
{
	int l_indx ;
  while(1){
  /* fill in the parameters : input_buffer */
  memset(input_buffer, 0, sizeof(input_buffer));
  for(l_indx = 0 ; l_indx < nb_Of_Input_Elements - 1 ; l_indx++)
  {
  	strcat(input_buffer, inputs_Array_Of_Elements[l_indx].value );
  	strcat(input_buffer, ":");
  }
  strcat(input_buffer, inputs_Array_Of_Elements[l_indx].value );
  
    /* fill in the parameters : output_buffer */
  memset(output_buffer, 0, sizeof(output_buffer));
  for(l_indx = 0 ; l_indx < nb_Of_Output_Elements  - 1 ; l_indx++)
  {
  	strcat(output_buffer, outputs_Array_Of_Elements[l_indx].value );
  	strcat(output_buffer, ":");
  }
  strcat(output_buffer, outputs_Array_Of_Elements[l_indx].value );
  
    /* fill in the parameters : message_buffer */
  memset(message_buffer, 0, sizeof(message_buffer));
  for(l_indx = 0 ; l_indx < nb_OF_ACTIVE_MESSAGES - 1 ; l_indx++)
  {
  	if ( active_message_list[l_indx].id_message != NO_ACTIVE_MESSAGE )
			{
		  	strcat(message_buffer, messages_list_cst [active_message_list[l_indx].id_message]  );
		  	strcat(message_buffer, ",+");
			}
  }
  if ( active_message_list[l_indx].id_message != NO_ACTIVE_MESSAGE )
  {
  	strcat(message_buffer, messages_list_cst [active_message_list[l_indx].id_message]  );
  }
  
  sprintf(message,http_request,private_key,input_buffer,message_buffer,output_buffer);

  /* create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
  	perror("ERROR opening socket");
  	exit(0);
  }

  /* lookup the ip address */
  server = gethostbyname(host);
  if (server == NULL)
  {
  	perror("ERROR, no such host");
  	exit(0);
  }

  /* fill in the structure */
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  memcpy(&serv_addr.sin_addr.s_addr,server->h_addr_list[0],server->h_length);

  /* connect the socket */
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
      error("ERROR connecting");

  /* send the request */
  total = strlen(message);
  sent = 0;
  do {
      bytes = write(sockfd,message+sent,total-sent);
      if (bytes < 0)
      {
          perror("ERROR writing message to socket");
          exit(0);
      }
      if (bytes == 0)
          break;
      sent+=bytes;
  } while (sent < total);

  /* receive the response */
  memset(response,0,sizeof(response));
  total = sizeof(response)-1;
  received = 0;
  do {
      bytes = read(sockfd,response-received,total-received);
      if (bytes < 0)
      {
          perror("ERROR reading response from socket");
          exit(0);
      }
      if (bytes == 0)
          break;
      received+=bytes;
  } while (received < total);

  if (received == total)
  {
  	perror("ERROR storing complete response from socket");
  	exit(0);
  }

  /* close the socket */
  close(sockfd);

 /* process response */
 e1 = strstr(response, "\r\n\r\n");
 if(e1 != NULL)
 {
  e2 = strstr(e1, "1");
  if (e2 == NULL)
  e2 = strstr(e1, "0");
  
  if (e2[0] == '0')
  {
		for(l_indx = 0 ; l_indx < nb_OF_ACTIVE_MESSAGES ; l_indx++)
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
  else if (e2[0] == '0')
  {
  	
  }
 }
 sleep(10);
  }
}
