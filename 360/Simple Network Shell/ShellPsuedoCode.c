int main (int argc, char* argv[], char* env[]) {
	// From env[] find your path (environment variables)
	// ask for a command -> [ cmd a b c ]
	// parse the cmd out ->
	if (!strcmp(cmd, "cd") || !strcmp(cmd, "exit")) {
		// Execute in this program
	} else {
		// Find the program
		pid = fork();
		if (pid) {
			// Must be parent:
			pid = wait(&status)
		} else {
			// Must child
			// execve("DIR/cmd", argv, env)
			// print out the programs return status
		}
	}
	// Loop back
}