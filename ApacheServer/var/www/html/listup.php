<html>
<head>
        <title> TEST </title>
</head>
<body>
        TESTING...<br>

        <script language='javascript'>
          window.setTimeout('window.location.reload()',1000); // 1sec refresh
        </script>

                <?php
                        @mysql_connect("localhost", "root", "root") or
                            die("Could not connect: " . mysql_error());
                        mysql_select_db("tech_schem");

						// Time
                        $result = mysql_query("SELECT now();");
                        $serv_time = @mysql_fetch_array($result, MYSQL_NUM);
                        printf("servtime: %s",$serv_time[0]);
						
						// passengerinfo
                        $result = mysql_query("SELECT * FROM passengerinfo WHERE now() - reportTime <= 30;" );	// display less than 30sec gap 

                         echo ('<br><table border=1> <tr><th> passengerid </th><th> batterylevel </th><th> rssi </th> <th> sentinelID </th> <th> reporttime </th><th> illumination </th></tr>');
                        while ($row = @mysql_fetch_array($result, MYSQL_NUM)) {
                                echo ('<tr>');
                                        for($i=0;$i<sizeof($row);$i++){         
                                                echo ('<th>');
                                                printf("%s",$row[$i]);
                                                echo ('</th>');
                                        }
                                echo ('</tr>');
                        }
                        echo ('</table>');

						// sentinels
                        $result = mysql_query("SELECT * FROM sentinels;");
                         echo ('<br><table border=1> <tr><th> sentinelID </th><th> status </th><th> sentinelname</th></tr>');

                        while ($row = @mysql_fetch_array($result, MYSQL_NUM)) {
                                echo ('<tr>');
                                        for($i=0;$i<sizeof($row);$i++){
                                                echo ('<th>');
                                                if ($row[$i] == NULL)
                                                        printf(" NULL ");
                                                else
                                                        printf("%s",$row[$i]);
                                                echo ('</th>');
                                        }
                                echo ('</tr>');
                        }
                        echo ('</table>');

						// passengers
                        $result = mysql_query("SELECT * FROM passengers;");
                        echo ('<br><table border=1> <tr><th> pid </th><th> activation </th></tr>');

                        while ($row = @mysql_fetch_array($result, MYSQL_NUM)) {
                                echo ('<tr>');
                                        for($i=0;$i<sizeof($row);$i++){
                                                echo ('<th>');
                                                 if ($row[$i] == NULL)
                                                        printf(" NULL ");
                                                else
                                                        printf("%s",$row[$i]);
                                                echo ('</th>');
                                        }
                                echo ('</tr>');
                        }
                        echo ('</table>');
						
						// free_data
                        mysql_free_result($result);
                ?>
</body>
</html>
