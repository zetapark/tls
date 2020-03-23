#!/bin/bash

awk '{
	if ($1 == "Resumption") print $3
	else if ($1 == "0000") print $3, $4, $5, $6, $7, $8, $9, $10
}' < <(openssl s_client < <(sleep 0.3) )
