<?php
$row = 1;
if (($handle = fopen("/var/www/drupal-multisite/sites/www.swazilii.org/SwaziLII-Saflii/SwaziLII_Metadata_final_2011.csv", "r")) !== FALSE) {
    while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
        $num = count($data);
        echo "<p> $num fields in line $row: <br /></p>\n";
        $row++;
        for ($c=0; $c < $num; $c++) {
            echo $data[$c] . "<br />\n";
        }
    }
    fclose($handle);
}

?>
