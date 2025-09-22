local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

-- ClientData / Inventory
local ClientData = require(ReplicatedStorage:WaitForChild("SharedModules"):WaitForChild("ClientData"))
local InventoryDB = ClientData.get("inventory")

-- API Remote
local API = ReplicatedStorage:WaitForChild("API"):WaitForChild("EmtsjpjEUOGIlCtGnGtYDKxKCJNdSQNJVXVJ")

-- Path to recycler in the world
local HouseInteriors = workspace:WaitForChild("HouseInteriors")

-- Get pets that are Common → UltraRare
local function getEligiblePets()
    local pets = InventoryDB and InventoryDB.pets or {}
    local selected = {}
    for uniqueId, petData in pairs(pets) do
        if petData and petData.rarity then
            local rarity = string.lower(petData.rarity)
            if rarity == "common" or rarity == "uncommon" or rarity == "rare" or rarity == "ultrarare" then
                table.insert(selected, uniqueId)
            end
        end
    end
    return selected
end

-- Check if recycler is ready
local function isRecyclerReady()
    local recycler = HouseInteriors:FindFirstChild("furniture")
    if not recycler then return false end

    local recyclerModel
    for _, v in ipairs(recycler:GetDescendants()) do
        if v.Name == "PetRecycler" then
            recyclerModel = v
            break
        end
    end
    if not recyclerModel then return false end

    local releaseText = recyclerModel:FindFirstChild("ReleaseText", true)
    if releaseText and releaseText:IsA("TextLabel") then
        return releaseText.Text == "READY"
    end

    local timerSign = recyclerModel:FindFirstChild("TimerSign", true)
    if timerSign then
        local gui = timerSign:FindFirstChildWhichIsA("SurfaceGui")
        if gui then
            local label = gui:FindFirstChildWhichIsA("TextLabel")
            if label and label.Text == "READY" then
                return true
            end
        end
    end
    return false
end

-- Claim rewards
local function claimRewards()
    local args = {
        "f-27",
        "UseBlock",
        { action = "claim" },
        LocalPlayer.Character
    }
    API:InvokeServer(unpack(args))
    print("Claimed recycler rewards.")
end

-- Recycle pets
local function recyclePets()
    local petsToRecycle = getEligiblePets()
    if #petsToRecycle > 0 then
        print("Recycling", #petsToRecycle, "pets...")
        for _, uid in ipairs(petsToRecycle) do
            API:InvokeServer("f-27", "UseBlock", {
                action = "use",
                uniques = { uid }
            }, LocalPlayer.Character)
            task.wait(1) -- avoid spamming
        end
    else
        print("No eligible pets to recycle.")
    end
end

-- Main loop
task.spawn(function()
    while task.wait(2) do
        if isRecyclerReady() then
            -- Step 1: Claim any rewards
            claimRewards()
            task.wait(1)

            -- Step 2: Recycle pets
            recyclePets()
        end
    end
end)
