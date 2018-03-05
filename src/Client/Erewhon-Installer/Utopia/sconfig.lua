local passwordFile = io.open("passwordFile.txt")
assert(passwordFile)

local saltFile = io.open("saltFile.txt")
assert(saltFile)

Database = {
	Host = "malcolm.digitalpulsesoftware.net",
	Port = 5432,
	Name = "utopia",
	Username = "lynix",
	Password = passwordFile:read(),
	WorkerCount = 2
}

Game = {
	MaxClients = 100,
	Port       = 2049,
	WorkerCount = 2
}

-- Warning: changing these parameters will break login to already registered accounts
Security = {
	Argon2 = {
		IterationCost = 10,
		MemoryCost    = 4 * 1024,
		ThreadCost    = 1
	},
	HashLength          = 32,
	PasswordSalt = saltFile:read(),
}
