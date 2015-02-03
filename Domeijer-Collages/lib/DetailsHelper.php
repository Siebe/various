<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of DeltailsHelper
 *
 * @author User
 */
class DetailsHelper {
    //put your code here
    
    static public function listDetails($details_list, $no_echo = false) {
        $result = '<div class="details-list">'."\n";
        if (!isset($details_list) || sizeof($details_list) < 1)  {
            $result .= '<p>Nog geen extra afbeeldingen toegevoegd.</p>'."\n";
        } else {
            $result .= '<p>Extra afbeelding(en):</p>'."\n";
            foreach ($details_list as $details) {
                $result .= '<div class="details-image image-preview">'."\n";
                $result .= '<img class="detals" src="'.$details->image.'">'."\n";
                $result .= '</div>';
            }
        }
        $result .= '</div>';
        if (!$no_echo) {
            echo $result;
        }
        return $result;

    }

}

?>
