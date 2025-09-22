local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer
local Workspace = game:GetService("Workspace")

local API = ReplicatedStorage:WaitForChild("API")
local LegacyLoad = require(ReplicatedStorage:WaitForChild("LegacyLoad"))
local ClientData = LegacyLoad("ClientData")

-- 🔎 detect random recycler remote
local function getRecyclerRemote()
	for _, obj in ipairs(API:GetChildren()) do
		if obj:IsA("RemoteFunction") then
			local success, _ = pcall(function()
				obj:InvokeServer("f-27","UseBlock",{action="test"}, LocalPlayer.Character)
			end)
			if success then
				return obj
			end
		end
	end
end

-- ♻️ send recycler
local function sendRecycler(remote, uniques)
	local args = {
		"f-27",
		"UseBlock",
		{
			action = "use",
			uniques = uniques or {}
		},
		LocalPlayer.Character
	}
	pcall(function()
		remote:InvokeServer(unpack(args))
	end)
end

-- 🐾 pick pets to recycle
local function getPetsToRecycle()
	local inventory = ClientData.get("inventory")
	local pets = inventory and inventory.pets or {}
	local list = {}
	for unique, pet in pairs(pets) do
		local rarity = pet.rarity
		if rarity == "Common" or rarity == "Uncommon" or rarity == "Rare" or rarity == "Ultra" then
			table.insert(list, unique)
		end
	end
	return list
end

-- ⏱ wait for the recycler to be READY
local function waitForReadyLabel()
	local timerLabel = Workspace:WaitForChild("HouseInteriors")
		:FindFirstChild("furniture", true)
		:FindFirstChildWhichIsA("SurfaceGui", true)
	local label = timerLabel and timerLabel:FindFirstChildWhichIsA("TextLabel", true)
	if not label then
		warn("⚠️ Couldn’t find timer label")
		return nil
	end
	return label
end

-- 🔄 main loop
task.spawn(function()
	local recyclerRemote = getRecyclerRemote()
	if not recyclerRemote then
		warn("⚠️ Recycler remote not found")
		return
	end

	local label = waitForReadyLabel()
	if not label then return end

	while true do
		-- wait until "READY"
		repeat task.wait(1) until label.Text == "READY"

		-- collect pets
		local petsToRecycle = getPetsToRecycle()
		sendRecycler(recyclerRemote, petsToRecycle)
		print("♻️ Recycled", #petsToRecycle, "pets (plus points if any)")

		-- small buffer to avoid double-fire
		task.wait(1)
	end
end)
