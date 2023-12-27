/*
 * AstCC: Asterisk Coin Controller Interface
 *
 * Copyright (C) 2022, Naveen Albert
 *
 * Naveen Albert <asterisk@phreaknet.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*! \file
 *
 * \brief AstCC: Asterisk Coin Controller Interface
 *
 * \author Naveen Albert <asterisk@phreaknet.org>
 */

/*
 * This is a boilerplate example of a simple AMI interface for
 * coin lines between Asterisk and a physical coin controller.
 *
 * Compilation Instructions:
 * This program needs to be statically compiled with CAMI.
 * The latest source for CAMI can be downloaded from: https://github.com/InterLinked1/cami
 * The following files are required (with this folder hierarchy):
 * - cami.c
 * - include/cami.h
 * - include/cami_actions.h
 * (simpleami.c and Makefile from CAMI are not needed)
 *
 * Your folder hierarchy should this look like this:
 * - include/cami.h (from CAMI)
 * - include/cami_actions.h (from CAMI)
 * - cami.c (from CAMI)
 * - astcc.c (from AstCC)
 * - Makefile (from AstCC)
 *
 * To compile, simply run "make".
 *
 * You will need to run the program under a user with permissions to access the serial port
 * or any other resources to which you may need access.
 * If the user has access to /etc/asterisk/manager.conf, you do not need to explicitly
 * specify the password as this program will read the config file and obtain it for you,
 * which is recommended over specifying it as an argument (IF the run user has access to that file).
 *
 * Program Dependencies:
 * - CAMI:    https://github.com/InterLinked1/cami
 * - Asterisk: res_coindetect module
 * - Asterisk manager.conf configuration:
 * -- Must have an AMI user with sufficient read/write permissions (call read/write).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>

#include <cami/cami.h>
#include <cami/cami_actions.h>

/*! \brief Use device names rather than full channel names for operations. Comment this out if you don't want this. */
#define DEVICE_NAME_ONLY

/*! \todo
 * Complete these function stubs to work appropriately for your coin controller.
 * You can use C I/O functions to interact with a serial line, for example.
 * You will need to use the channel name (or device name) to find the appropriate
 * port if you have more than one coin phone you are controlling using this program.
 *
 * Return 0 on success and return -1 if a failure occurs and the operation
 * could not be completed.
 */

/*! \brief -130 volt coin return */
static int coin_return(const char *channel)
{
	/*! \todo Coin return */
	return 0;
}

/*! \brief +130 volt coin collect */
static int coin_collect(const char *channel)
{
	/*! \todo Coin collect */
	return 0;
}

/*! \brief Enable DTMF keypad (negative battery) */
static int operator_released(const char *channel)
{
	/*! \todo Operator released */
	return 0;
}

/*! \brief Disable DTMF keypad (positive battery) */
static int operator_attached(const char *channel)
{
	/*! \todo Operator attached */
	return 0;
}

/*! \brief Ring back the pay station */
/*! \note Not expected to be used, included for completeness */
static int operator_ringback(const char *channel)
{
	return 0;
}

/*! \brief Callback function executing asynchronously when new events are available */
static void ami_callback(struct ami_session *ami, struct ami_event *event)
{
	int res = 0;
	const char *disp, *channel, *eventname = ami_keyvalue(event, "Event");

#ifdef DEVICE_NAME_ONLY
	char *tmp;
	char device_name[64];
#endif

	(void) ami;

	if (strcmp(eventname, "CoinDisposition")) {
		goto cleanup; /* Don't care about anything else. */
	}

	channel = ami_keyvalue(event, "Channel");
	disp = ami_keyvalue(event, "Disposition");

	assert(channel != NULL);
	assert(disp != NULL);

	printf("Coin event on channel %s: %s\n", channel, disp);

	/* Individual controller logic here: this will vary from system to system.
	 * You might use a serial interface to the controller to tell it what to do.
	 * Or something else entirely... */

	/* Note: channel here is the full channel name in Asterisk. You MAY very well want just the device name, or even something else entirely.
	 * In most cases, you probably want just the device name so defining DEVICE_NAME_ONLY can be used to enable that behavior.
	 */

#ifdef DEVICE_NAME_ONLY
	strncpy(device_name, channel, sizeof(device_name));
	device_name[sizeof(device_name) - 1] = '\0'; /* On the off chance we filled the entire buffer, always null terminate */
	tmp = strrchr(device_name, '-'); /* See ast_channel_name_to_dial_string in Asterisk */
	if (!tmp) {
		fprintf(stderr, "Invalid device name: %s\n", device_name);
		goto cleanup;
	}
	*tmp = '\0';
	channel = device_name;
#endif

	if (!strcmp(disp, "CoinReturn")) {
		res = coin_return(channel);
	} else if (!strcmp(disp, "CoinCollect")) {
		res = coin_collect(channel);
	} else if (!strcmp(disp, "OperatorRingback")) {
		res = operator_ringback(channel);
	} else if (!strcmp(disp, "OperatorReleased")) {
		res = operator_released(channel);
	} else if (!strcmp(disp, "OperatorAttached")) {
		res = operator_attached(channel);
	} else if (!strcmp(disp, "CoinCollectOperatorReleased")) {
		res |= coin_collect(channel);
		res |= operator_released(channel);
	}

	if (res) {
		fprintf(stderr, "Coin operation (%s) failed on channel %s\n", disp, channel);
	}

cleanup:
	ami_event_free(event); /* Free event when done with it */
}

static void simple_disconnect_callback(struct ami_session *ami)
{
	/* Start with a newline, since we don't know where we were. */
	(void) ami;
	fprintf(stderr, "\nAMI was forcibly disconnected...\n");
	exit(EXIT_FAILURE); /* This will kill the program. */
}

static void show_help(void)
{
	printf("AstCC for Asterisk\n");
	printf(" -h           Show this help\n");
	printf(" -l           Asterisk AMI hostname. Default is localhost (127.0.0.1)\n");
	printf(" -p           Asterisk AMI password. By default, this will be autodetected for local connections if possible.\n");
	printf(" -u           Asterisk AMI username.\n");
	printf("(C) 2022 Naveen Albert\n");
}

int main(int argc,char *argv[])
{
	char c;
	static const char *getopt_settings = "?hl:p:u:";
	char ami_host[92] = "127.0.0.1"; /* Default to localhost */
	char ami_username[64] = "";
	char ami_password[64] = "";
	struct ami_session *ami;

	while ((c = getopt(argc, argv, getopt_settings)) != -1) {
		switch (c) {
		case '?':
		case 'h':
			show_help();
			return 0;
		case 'l':
			strncpy(ami_host, optarg, sizeof(ami_host));
			break;
		case 'p':
			strncpy(ami_password, optarg, sizeof(ami_password));
			break;
		case 'u':
			strncpy(ami_username, optarg, sizeof(ami_username));
			break;
		default:
			fprintf(stderr, "Invalid option: %c\n", c);
			return -1;
		}
	}

	if (ami_username[0] && !ami_password[0] && !strcmp(ami_host, "127.0.0.1")) {
		/* If we're running as a privileged user with access to manager.conf, grab the password ourselves, which is more
		 * secure than getting as a command line arg from the user (and kind of convenient)
		 * Not that running as a user with access to the Asterisk config is great either, but, hey...
		 */
		if (ami_auto_detect_ami_pass(ami_username, ami_password, sizeof(ami_password))) {
			fprintf(stderr, "No password specified, and failed to autodetect from /etc/asterisk/manager.conf\n");
			return -1;
		}
	}

	if (!ami_username[0]) {
		fprintf(stderr, "No username provided (use -u flag)\n");
		return -1;
	}

	ami = ami_connect(ami_host, 0, ami_callback, simple_disconnect_callback);
	if (!ami) {
		return -1;
	}
	if (ami_action_login(ami, ami_username, ami_password)) {
		fprintf(stderr, "Failed to log in with username %s\n", ami_username);
		return -1;
	}

	/* This thread will now sleep forever, since all logic is in the callback. */
	for (;;) {
		usleep(60000000);
	}

	ami_disconnect(ami);
	ami_destroy(ami);
	return 0;
}
