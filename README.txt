
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
* XML hublist generation
* Hubs which are offline too many times are not pinged after a time
* Users with permission can ban a hub
* Option to filter duplicated hubs from the list
* Debug window to manually ping a hub and see the protocol messages
* Views support (required to create a summarized table for the hubs on the web)

Prerequirements
---------------
* Drupal 7.x
* Date API module (included in http://drupal.org/project/date/)

Optional
--------
* Views module (http://drupal.org/project/views)
  - Views module is only optional, but strongly recommended since you
    can't create a web-based summarized table of the hubs without that

Installation
------------
1) Copy the content of this directory in /sites/all/modules/hublist
2) Go to /admin/modules then enable Direct Connect Hublist
3) Go to /admin/people/permissions and configure the rights for the hublist
   module
4) Go to /admin/config/services/hublist to configure the settings of the list
5) Don't forget to configure the cron job since the pinger needs that

Reporting bugs and contact
--------------------------
Currently the project is hosted on github.com, so you can always get the
latest version by visiting the project page. Whenever you find a bug,
or you have a question or recommendation, feel free to open an Issue.
 * https://github.com/fleetcommand/hublist

Copyright
---------
The Hublist module is licensed under the terms of GNU General Public License
version 3. For details and full text of license, see the LICENSE.txt included
with this package or visit http://www.gnu.org/

Currently the module includes contributions from the following people:
(C) Viktor Balázs, 2009-2010
(C) Szabolcs Molnár, 2009-2011