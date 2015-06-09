<html>
<head>
	<title> TEST </title>
</head>

<body>
	<?php
			@mysql_connect("localhost", "root", "root") or
			    die("Could not connect: " . mysql_error());
			mysql_select_db("tech_schem");
	
			$q = $_GET["_id"];			
			$result = mysql_query("SELECT sentinelID from sentinels WHERE ". $q . ";");
			$sen_id = mysql_result($result,0,"sentinelID");
			
			if($sen_id!=$q) 
			{					
				$result = mysql_query("INSERT INTO sentinels (sentinelID) VALUES (". $q . ");");
				echo ("\"".$q."\""." Registration Clear!<br>");
			}			
			else	
			{
				echo ("\"".$q."\""." Registration Fail!<br>Already in DB!<br>");
			}				
			mysql_free_result($result); 
			
			echo ("<br>Moving to list page...");
			echo ("<meta http-equiv='refresh' content='2;url=./listup.php'>");
	?>
	
</body>
</html>