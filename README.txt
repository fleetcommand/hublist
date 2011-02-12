
                            DIRECT CONNECT HUBLIST
            A Drupal module for creating online Direct Connect hublist

Introduction
------------
This module enables you to create a Drupal-based hublist for the Direct Connect
network. With this software, you will be able to create an automatically
updating list which can be viewed both online and from any modern DC client.

Features
--------
* Hublist pinger with NMDC and ADC support
* Hubs which are offline too many times are not pinged after a time
* Users with permission can ban a hub
* Option to filter duplicated hubs from the list
* XML hublist generation
* Views support (required to create a summarized table for the hubs on the web)

Prerequirements
---------------
* Drupal 6.x
* Date API module (included in http://drupal.org/project/date/)
* Views module (http://drupal.org/project/views)
  - Actually Views module is only optional, but strongly recommended since
    you can't create a web-based summarized table of the hubs without that

Installation
------------
1) Copy the content of this directory in /sites/all/modules/hublist
2) Go to /admin/build/modules then enable Direct Connect Hublist
3) Go to /admin/user/permissions and configure the rights for the hublist
   module
4) Go to /admin/build/hublist to configure the settings of the list
5) Don't forget to configure the cron job since the pinger needs that

Reporting bugs and contact
--------------------------
The SVN and the bug tracking system is hosted at SourceForge, so if you find
any bugs or have any recommendation or question, visit the following site:
 * http://drupal.org/url_is_going_to_come_here

Known issues
------------
Due to a bug in Date API module, the Last ping date is set to 1970-01-01 when
you add that field to your view. Note that this bug does not affect the hub's
datasheet, only happens when you add the Last ping date field to your view.

Copyright
---------
The Hublist module is licensed under the terms of GNU General Public License,
you can redistribute and/or modify it under the terms of the license as published
by the Free Software Foundation; either version 2 of the License, or (at your option)
any later version. For details and full text of license, see the LICENSE.txt included
with this package or visit http://www.gnu.org/

Currently the module includes contributions from the following people:
(C) Viktor Balázs, 2009-2010
(C) Szabolcs Molnár, 2009-2011