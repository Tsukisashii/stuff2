local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

local API = ReplicatedStorage:WaitForChild("API")
local LegacyLoad = require(ReplicatedStorage:WaitForChild("LegacyLoad"))
local ClientData = LegacyLoad("ClientData")

-- 🔎 find recycler remote
local function getRecyclerRemote()
    for _, obj in ipairs(API:GetChildren()) do
        if obj:IsA("RemoteFunction") then
            return obj
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
    return remote:InvokeServer(unpack(args))
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

-- 🔄 main loop
task.spawn(function()
    local recyclerRemote = getRecyclerRemote()
    if not recyclerRemote then
        warn("⚠️ Recycler remote not found")
        return
    end

    -- find timer label
    local timerLabel = workspace:WaitForChild("HouseInteriors")
        :FindFirstChild("furniture", true)
        :FindFirstChildWhichIsA("SurfaceGui", true)
    local label = timerLabel and timerLabel:FindFirstChildWhichIsA("TextLabel", true)

    if not label then
        warn("⚠️ Couldn’t find timer label")
        return
    end

    while true do
        -- wait until "READY"
        repeat task.wait(1) until label.Text == "READY"

        -- collect pets
        local petsToRecycle = getPetsToRecycle()
        print("♻️ Recycling...", #petsToRecycle, "pets (plus points if any)")

        -- recycle pets or points
        sendRecycler(recyclerRemote, petsToRecycle)

        -- wait a bit so we don’t double-fire
        task.wait(2)
    end
end)
