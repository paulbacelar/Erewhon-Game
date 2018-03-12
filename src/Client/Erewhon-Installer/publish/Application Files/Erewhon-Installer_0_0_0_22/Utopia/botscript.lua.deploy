local HeadingController = Vec3Pid.New(1, 0.0, 0.6382979)

Spaceship.TimeSinceStart = 0
Spaceship.TargetChangeAccumulator = 0
Spaceship.CurrentScanDirection = 0
Spaceship.CurrentTarget = nil

function Spaceship:OnStart()
end

function Spaceship:OnTick(elapsedTime)
	self.TimeSinceStart = self.TimeSinceStart + elapsedTime
	
	self:PerformConeScan()

	self.TargetChangeAccumulator = self.TargetChangeAccumulator + elapsedTime
	if (self.TargetChangeAccumulator > 5) then
		if (self.CurrentTarget) then
			self.Navigation:FollowTarget(self.CurrentTarget.id, 20)
		end

		self.TargetChangeAccumulator = 0
	end
end

function Spaceship:OnNavigationDestinationReached()
	print("Destination reached!")
end

function Spaceship:OnRadarConeScanReady()
	assert(self.Radar:IsConeScanReady())
	self:PerformConeScan()
end

function Spaceship:OnRadarTargetScanReady()
	assert(self.Radar:IsTargetScanReady())
	self:PerformTargetScan()
end

local scanDirections = {Vec3.Forward, Vec3.Left, Vec3.Backward, Vec3.Right, Vec3.Up, Vec3.Down}

function Spaceship:PerformConeScan()
	if (self.Radar:IsConeScanReady()) then
		local result = self.Radar:ScanInCone(scanDirections[self.CurrentScanDirection + 1])

		-- Use results
		local currentPos = self.Core:GetPosition()

		if (self.CurrentTarget) then
			if (self.CurrentTargetData) then
				self.CurrentTarget.position = self.CurrentTargetData.position
			end

			table.insert(result, self.CurrentTarget)
		end

		local bestResult
		local bestResultDis
		for k,v in pairs(result) do
			if (v.type == "spaceship") then
				local distSq = v.position:SquaredDistance(currentPos)
				if (not bestResult or distSq < bestResultDis) then
					bestResult = v
					bestResultDis = distSq
				end
			end
		end

		self.CurrentTarget = bestResult

		self.CurrentScanDirection = (self.CurrentScanDirection + 1) % 6
	end
	
	self:PerformTargetScan()
end

function Spaceship:PerformTargetScan()
	if (self.Radar:IsTargetScanReady() and self.CurrentTarget) then
		self.CurrentTargetData = self.Radar:ScanTarget(self.CurrentTarget.id)
	end
end
