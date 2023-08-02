/*
CoSiM Demo Using the Firewall Module as DuT 
Copyright (C) TEI Crete

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <sys/types.h> //used for gid_t 
#include <unistd.h>    //used symbolic constants and types NGROUPS_MAX
#include <limits.h>

char *          /* Return name corresponding to 'uid', or NULL on error */
userNameFromId(uid_t uid)
{
    struct passwd *pwd;

    pwd = getpwuid(uid);
    return (pwd == NULL) ? NULL : pwd->pw_name;
}
uid_t           /* Return UID corresponding to 'name', or -1 on error */
userIdFromName(const char *name)
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    if (name == NULL || *name == '\0')  /* On NULL or empty string */
        return -1;                      /* return an error */

    u = strtol(name, &endptr, 10);      /* As a convenience to caller */
    if (*endptr == '\0')                /* allow a numeric string */
        return u;

    pwd = getpwnam(name);
    if (pwd == NULL)
        return -1;

    return pwd->pw_uid;
}
char *          /* Return name corresponding to 'gid', or NULL on error */
groupNameFromId(gid_t gid)
{
    struct group *grp;

    grp = getgrgid(gid);
    return (grp == NULL) ? NULL : grp->gr_name;
}

gid_t groupIdFromName(const char *name)
{
    struct group *grp;
    gid_t g;
    char *endptr;

    if (name == NULL || *name == '\0')  /* On NULL or empty string */
        return -1;                      /* return an error */

    g = strtol(name, &endptr, 10);      /* As a convenience to caller */
    if (*endptr == '\0')                /* allow a numeric string */
        return g;

    grp = getgrnam(name);
    if (grp == NULL)
        return -1;

    return grp->gr_gid;
}

//Function that return grouplist
int grouplist(gid_t *groups, int *ngroups)
{
   register struct passwd *pw;
   register uid_t uid;

   uid = geteuid();	//get effective user
   pw = getpwuid(uid);	//get password entry of efective user
   if(pw){   

   	if ( getgrouplist( pw->pw_name, pw->pw_gid, groups, ngroups) == -1) {
		   printf ("Groups array is too small: %d\n", *ngroups);
   	}

   	//printf ("%s belongs to these groups: %d  ", pw->pw_name, pw->pw_gid);
   	//for (i=0; i < *ngroups; i++) {
	//   printf (", %d", groups[i]);
   	//}
   	//printf ("\n");
   
   	return EXIT_SUCCESS;

   }else{

	printf("Cannot find username for UID:%u\n",(unsigned)uid);
	return EXIT_FAILURE;

   }
}

int is_admin()
{
	int systemadmin_flag = 0, i = 0;
	int ngroups = NGROUPS_MAX;
	gid_t list_of_groups[ngroups];
	gid_t gid_systemadmin;

	// get systemadmin groupid
	gid_systemadmin = groupIdFromName("root"); 

	//check if current user belongs to systemadmin group (root)
	grouplist(list_of_groups, &ngroups); //call function grouplist to take in which groups belong each current user 
	for (i=0; i<ngroups; i++){
		if (list_of_groups[i] == gid_systemadmin) {
			systemadmin_flag = 1;
			break;
		}
	}
	return systemadmin_flag;
}


