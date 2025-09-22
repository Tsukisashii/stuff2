local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

-- 🕒 WaitForChild with timeout
local function safeWait(parent, childName, timeout)
    local obj = parent:WaitForChild(childName, timeout)
    if not obj then
        warn("⚠️ "..childName.." not found after "..timeout.." seconds")
    end
    return obj
end

local API = safeWait(ReplicatedStorage, "API", 5)
if not API then return end

local LegacyLoadModule = safeWait(ReplicatedStorage, "LegacyLoad", 5)
if not LegacyLoadModule then return end
local LegacyLoad = require(LegacyLoadModule)
local ClientData = LegacyLoad("ClientData")

-- 🔎 Detect recycler remote (random name each join)
local function getRecyclerRemote()
    for _, obj in ipairs(API:GetChildren()) do
        if obj:IsA("RemoteFunction") then
            return obj
        end
    end
    warn("⚠️ Recycler remote not found")
end

-- ♻️ Send recycler request
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
    return remote:InvokeServer(unpack(args))
end

-- 🐾 Pick pets to recycle (Common → Ultra)
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

-- 🔄 Main auto-recycler loop
task.spawn(function()
    local recyclerRemote = getRecyclerRemote()
    if not recyclerRemote then return end

    -- Find the recycler timer label
    local timerGui = workspace:WaitForChild("HouseInteriors", 5)
        :FindFirstChild("furniture", true)
        :FindFirstChildWhichIsA("SurfaceGui", true)
    local timerLabel = timerGui and timerGui:FindFirstChildWhichIsA("TextLabel", true)

    if not timerLabel then
        warn("⚠️ Couldn’t find recycler timer label")
        return
    end

    while true do
        -- Wait until "READY"
        repeat task.wait(1) until timerLabel.Text == "READY"

        -- Collect pets to recycle
        local petsToRecycle = getPetsToRecycle()

        if #petsToRecycle > 0 then
            print("♻️ Recycling pets:", #petsToRecycle)
            sendRecycler(recyclerRemote, petsToRecycle)
        else
            print("♻️ No pets, recycling points only")
            sendRecycler(recyclerRemote, {}) -- still sends points
        end

        -- Small delay to prevent double-fire
        task.wait(2)
    end
end)
