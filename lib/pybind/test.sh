#!/bin/bash

#awk 'BEGIN {ORS = ""}
#	{ 
#		if($1 == "Resumption" && $2 == "PSK:") {
#			print $3;
#			print "\n"
#		} else if($1 ~ "00[a-f0-9]0" && $2 == "-" ) {
#			for(i=3; i<18; i++) print $i;
#			print "\n"
#		}
#	}' < <(openssl s_client -ciphersuites "TLS_AES_128_GCM_SHA256" < <(sleep 0.3) )

awk '{
       if ($1 == "Resumption") print $3
       else if ($1 == "0000") print $3, $4, $5, $6, $7, $8, $9, $10
}' < <(openssl s_client < <(sleep 0.3) )                	
