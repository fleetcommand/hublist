<?php
// $Id$

 /*
   Variables:

     $hub_url
     $hub_description
     $hub_important
     $hub_website
     $hub_owneremail
     $hub_status
     $hub_status_num
     $hub_status_comment
     $stat_pingdate
     $stat_hubsoftware
     $stat_usercount
     $stat_userlimit
     $stat_shared_formatted
     $stat_minshare_formatted
     $stat_minslots
     $stat_maxhubs
     $stat_reliability

  */
?>
<?php
  print("<fieldset class=\"fieldgroup group-basic\">\n");
  print("<legend>" . t("Basic info") . "</legend>\n");
  print("<strong>" . t("Hub address") . ":</strong> <a href=\"$hub_url\">$hub_url</a>" . "<br />\n");
  print("<strong>" . t("Description") . ":</strong> " . $hub_description . "<br />\n");
  print("<strong>" . t("Successful ping rate") . ":</strong> " . $stat_reliability . "<br />\n");
  if ($stat_hubsoftware != '')
    print("<strong>" . t("Hub software") .":</strong> " . $stat_hubsoftware . "<br />\n");
  if ($hub_important != '')
    print("<strong>" . t("Notes") .":</strong> " . $hub_important . "\n");
  print("</fieldset>\n");

  if ($hub_website . $hub_owneremail != "") {
    print("<fieldset class=\"fieldgroup group-contact\">\n");
    print("<legend>" . t("Contact") . "</legend>\n");
    if ($hub_website != "")
      print("<strong>" . t("Website") .":</strong> <a href=\"$hub_website\">$hub_website</a>" . "<br />\n");
    if ($hub_owneremail != "")
      print("<strong>" . t("E-mail") .":</strong> " . $hub_owneremail . "\n");
    print("</fieldset>\n");
  }

  if ($hub_status_num == 0) {
    print("<fieldset class=\"fieldgroup group-statistics\">\n");
    print("<legend>" . t("Current statistics") . "</legend>\n");
    print("<strong>" . t("Usercount") .":</strong> " . $stat_usercount . "<br />\n");
    print("<strong>" . t("Minimum share") .":</strong> " . $stat_minshare_formatted . "<br />\n");
    print("<strong>" . t("User limit") .":</strong> " . $stat_userlimit . "<br />\n");
    print("<strong>" . t("Total share") .":</strong> " . $stat_shared_formatted . "<br />\n");
    print("<strong>" . t("Minimum slots") .":</strong> " . $stat_minslots . "<br />\n");
    print("<strong>" . t("Maximum hubs") .":</strong> " . $stat_maxhubs . "\n");
    print("</fieldset>\n");
  }

  print("<fieldset class=\"fieldgroup group-status\">\n");
  print("<legend>" . t("Status") . "</legend>\n");
  print("<strong>" . t("Hub status") .":</strong> " . $hub_status . "<br />\n");
  if ($hub_status_comment != '')
    print("<strong>" . t("Additional information") .":</strong> " . $hub_status_comment . "<br />\n");
  if ($hub_status_num != -1)
    print("<strong>" . t("Latest ping attempt") .":</strong> " . $stat_pingdate . "\n");
  print("</fieldset>\n");