/*
 * Example Contiki test script (JavaScript).
 * A Contiki test script acts on mote output, such as via printf()'s.
 * The script may operate on the following variables:
 *  Mote mote, int id, String msg
 */

/* Make test automatically fail (timeout) after 100 simulated seconds */
//TIMEOUT(100000); /* milliseconds. no action at timeout */
//TIMEOUT(100000, log.log("last msg: " + msg + "\n")); /* milliseconds. print last msg at timeout */
TIMEOUT(100000, log.log("Not converged\n")); /* milliseconds. print last msg at timeout */


//log.log("first mote output: '" + msg + "'\n");

//GENERATE_MSG(15000, "continue");
//YIELD_THEN_WAIT_UNTIL(msg.equals("continue"));

while (true) {
	    log.log(time + " ID:" + id + " " + msg + "\n");
		if (msg.equals("Periodic Statistics: convergence time ended + hops"))
			log.testOK();
    YIELD();
}

//log.testOK();
log.testFailed(); /* Report test failure and quit */
